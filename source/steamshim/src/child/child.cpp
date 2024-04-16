/*
Copyright (C) 2023 coolelectronics

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <SDL2/SDL_stdinc.h>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <stdlib.h>
#include <thread>

#include "../os.h"
#include "../steamshim.h"
#include "../steamshim_private.h"
#include "ServerBrowser.h"
#include "child_private.h"
#include "steam/isteamfriends.h"
#include "steam/isteammatchmaking.h"
#include "steam/isteamutils.h"
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"
#include "steam/steamclientpublic.h"

#include "../packet_utils.h"
#include "write_utils.h"

static bool GRunServer = false;
static bool GRunClient = false;

static ISteamUserStats *GSteamStats = NULL;
static ISteamUtils *GSteamUtils = NULL;
static ISteamUser *GSteamUser = NULL;
static AppId_t GAppID = 0;
static uint64 GUserID = 0;
static ISteamGameServer *GSteamGameServer = NULL;
ServerBrowser *GServerBrowser = NULL;
static time_t time_since_last_pump = 0;

static SteamCallbacks *GSteamCallbacks;

static void processRPC( steam_rpc_pkt_s *req, size_t size )
{
	switch( req->common.cmd ) {
		case RPC_PUMP: {
			time( &time_since_last_pump );
			if( GRunServer )
				SteamGameServer_RunCallbacks();
			if( GRunClient )
				SteamAPI_RunCallbacks();
			struct steam_rpc_shim_common_s recv;
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_AUTHSESSION_TICKET: {
			struct auth_session_ticket_recv_s recv;
			prepared_rpc_packet( &req->common, &recv );
			GSteamUser->GetAuthSessionTicket( recv.ticket, AUTH_TICKET_MAXSIZE, &recv.pcbTicket);
			write_packet( GPipeWrite, &recv, sizeof( struct auth_session_ticket_recv_s ));
			break;
		}
		case RPC_PERSONA_NAME: {
			const char *name = SteamFriends()->GetPersonaName();
			struct buffer_rpc_s recv;
			prepared_rpc_packet( &req->common, &recv );

			const uint32_t bufferSize = strlen( name ) + 1 + sizeof( struct buffer_rpc_s );
			writePipe( GPipeWrite, &bufferSize, sizeof( uint32_t ) );
			writePipe( GPipeWrite, &recv, sizeof( struct buffer_rpc_s ) );
			writePipe( GPipeWrite, name, strlen( name ) + 1 );
			break;
		}
		case RPC_REQUEST_LAUNCH_COMMAND: {
			uint8_t buffer[sizeof( struct buffer_rpc_s ) + 1024];
			struct buffer_rpc_s *recv = (struct buffer_rpc_s *)buffer;
			prepared_rpc_packet( &req->common, recv );
			const int numberBytes = SteamApps()->GetLaunchCommandLine( (char *)recv->buf, 1024 );
			assert( numberBytes > 0 );
			write_packet( GPipeWrite, &recv, sizeof( buffer_rpc_s ) + numberBytes );
			break;
		}
		case RPC_BEGIN_AUTH_SESSION: {
			const int result = GSteamGameServer->BeginAuthSession( req->begin_auth_session.authTicket, req->begin_auth_session.cbAuthTicket, (uint64)req->begin_auth_session.steamID );
			struct steam_result_recv_s recv;
			prepared_rpc_packet( &req->common, &recv );
			recv.result = result;
			write_packet( GPipeWrite, &recv, sizeof( steam_result_recv_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO: {
			GSteamGameServer->SetAdvertiseServerActive( req->server_info.advertise );
			GSteamGameServer->SetDedicatedServer( req->server_info.dedicated );
			GSteamGameServer->SetMaxPlayerCount( req->server_info.maxPlayerCount );
			GSteamGameServer->SetBotPlayerCount( req->server_info.botPlayerCount );
			struct steam_rpc_shim_common_s recv;
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_result_recv_s ) );
			break;
		}
		case RPC_ACTIVATE_OVERLAY: {
			SteamFriends()->ActivateGameOverlayToUser( "steamid", (uint64)req->open_overlay.id );
			break;
		}
		case RPC_UPDATE_SERVERINFO_GAME_DATA: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameData( (const char *)req->server_game_data.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_GAME_DESCRIPTION: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_description.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_GAME_TAGS: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_tags.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_MAP_NAME: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_map_name.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_MOD_DIR: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_mod_dir.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_PRODUCT: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_product.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_REGION: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_region.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_UPDATE_SERVERINFO_SERVERNAME: {
			struct steam_rpc_shim_common_s recv;
			GSteamGameServer->SetGameDescription( (const char *)req->server_name.buf );
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_SET_RICH_PRESENCE: {
			char* str[2] = { 0 };
			if( unpack_string_array_null( (char *)req->rich_presence.buf, size - sizeof( struct buffer_rpc_s ), str, 2 ) ) {
				SteamFriends()->SetRichPresence( str[0], str[1]);
			}
			struct steam_rpc_shim_common_s recv;
			prepared_rpc_packet( &req->common, &recv );
			write_packet( GPipeWrite, &recv, sizeof( steam_rpc_shim_common_s ) );
			break;
		}
		case RPC_END_AUTH_SESSION: {
			GSteamGameServer->EndAuthSession( (uint64)req->end_auth_session.id );
			break;
		}
		case RPC_REQUEST_STEAM_ID: {
			struct steam_id_rpc_s recv;
			prepared_rpc_packet( &req->common, &recv );
			recv.id = SteamUser()->GetSteamID().ConvertToUint64();
			write_packet( GPipeWrite, &recv, sizeof( steam_id_rpc_s ) );
			break;
		}
		case RPC_REQUEST_AVATAR: {
			uint8_t buffer[sizeof( struct steam_avatar_recv_s ) + STEAM_MAX_AVATAR_SIZE];
			struct steam_avatar_recv_s *recv = (struct steam_avatar_recv_s *)buffer;
			prepared_rpc_packet( &req->common, recv );
			int handle;
			switch( req->avatar_req.size ) {
				case STEAM_AVATAR_SMALL:
					handle = SteamFriends()->GetSmallFriendAvatar( (uint64)req->avatar_req.steamID );
					break;
				case STEAM_AVATAR_MED:
					handle = SteamFriends()->GetMediumFriendAvatar( (uint64)req->avatar_req.steamID );
					break;
				case STEAM_AVATAR_LARGE:
					handle = SteamFriends()->GetLargeFriendAvatar( (uint64)req->avatar_req.steamID );
					break;
				default:
					recv->width = 0;
					recv->height = 0;
					goto fail_avatar_image;
			}
			SteamUtils()->GetImageSize( handle, &recv->width, &recv->height );
			SteamUtils()->GetImageRGBA( handle, recv->buf, STEAM_MAX_AVATAR_SIZE );
		fail_avatar_image:
			const uint32_t size = ( recv->width * recv->height * 4 ) + sizeof( steam_avatar_recv_s );
			assert( size <= sizeof( buffer ) );
			writePipe( GPipeWrite, &size, sizeof( uint32_t ) );
			writePipe( GPipeWrite, buffer, size );
			break;
		}
		default:
			break;
	}
}
static void processCommands()
{
	static struct steam_packet_buf packet;
	static size_t cursor = 0;
	bool hasMore = false;
	while( 1 ) {
		if( time_since_last_pump != 0 ) {
			time_t delta = time( NULL ) - time_since_last_pump;
			if( delta > 5 ) // we haven't gotten a pump in 5 seconds, safe to assume the main process is either dead or unresponsive and we can terminate
				return;
		}

		assert( sizeof( struct steam_packet_buf ) == STEAM_PACKED_RESERVE_SIZE );

		const int bytesRead = readPipe( GPipeRead, packet.buffer + cursor, STEAM_PACKED_RESERVE_SIZE - cursor );
		if( bytesRead > 0 ) {
			cursor += bytesRead;
		} else {
			std::this_thread::sleep_for( std::chrono::microseconds( 1000 ) );
			continue;
		}
		continue_processing:

		if( packet.size > STEAM_PACKED_RESERVE_SIZE - sizeof( uint32_t ) ) {
			// the packet is larger then the reserved size
			return;
		}

		if( cursor < packet.size + sizeof( uint32_t ) ) {
			
			continue;
		}

		// readPipe(GPipeRead, &packet, size );
		if( packet.common.cmd >= RPC_BEGIN && packet.common.cmd < RPC_END ) {
			dbgprintf( "process packet: %lu %lu\n", packet.rpc_payload.common.cmd, packet.rpc_payload.common.sync );
			processRPC( &packet.rpc_payload, packet.size );
		}

		if( cursor > packet.size + sizeof( uint32_t ) ) {
			const size_t packetlen = packet.size + sizeof( uint32_t );
			const size_t remainingLen = cursor - packetlen;
			memmove( packet.buffer, packet.buffer + packet.size + sizeof( uint32_t ), remainingLen );
			cursor = remainingLen;
			goto continue_processing;
		} else {
			cursor = 0;
		}
	}
}

static bool initSteamworks( PipeType fd )
{
	if( GRunClient ) {
		// this can fail for many reasons:
		//  - you forgot a steam_appid.txt in the current working directory.
		//  - you don't have Steam running
		//  - you don't own the game listed in steam_appid.txt
		if( !SteamAPI_Init() )
			return 0;

		GSteamStats = SteamUserStats();
		GSteamUtils = SteamUtils();
		GSteamUser = SteamUser();

		GAppID = GSteamUtils ? GSteamUtils->GetAppID() : 0;
		GUserID = GSteamUser ? GSteamUser->GetSteamID().ConvertToUint64() : 0;

		GServerBrowser = new ServerBrowser();

		GServerBrowser->RefreshInternetServers();
	}

	if( GRunServer ) {
		// this will fail if port is in use
		if( !SteamGameServer_Init( 0, 27015, 27016, eServerModeAuthenticationAndSecure, "0.0.0.0" ) )
			return 0;
		GSteamGameServer = SteamGameServer();
		if( !GSteamGameServer )
			return 0;

		GSteamGameServer->LogOnAnonymous();
	}

	GSteamCallbacks = new SteamCallbacks();
	return 1;
}

static void deinitSteamworks()
{
	if( GRunServer ) {
		SteamGameServer_Shutdown();
	}

	if( GRunClient ) {
		SteamAPI_Shutdown();
	}
}

static int initPipes( void )
{
	char buf[64];
	unsigned long long val;

	if( !getEnvVar( "STEAMSHIM_READHANDLE", buf, sizeof( buf ) ) )
		return 0;
	else if( sscanf( buf, "%llu", &val ) != 1 )
		return 0;
	else
		GPipeRead = (PipeType)val;

	if( !getEnvVar( "STEAMSHIM_WRITEHANDLE", buf, sizeof( buf ) ) )
		return 0;
	else if( sscanf( buf, "%llu", &val ) != 1 )
		return 0;
	else
		GPipeWrite = (PipeType)val;

	return ( ( GPipeRead != NULLPIPE ) && ( GPipeWrite != NULLPIPE ) );
} /* initPipes */

int main( int argc, char **argv )
{
	//std::this_thread::sleep_for( std::chrono::microseconds( 5000) );
#ifndef _WIN32
	signal( SIGPIPE, SIG_IGN );

	if( argc > 1 && !strcmp( argv[1], "steamdebug" ) ) {
		debug = true;
	}
#endif

	dbgprintf( "Child starting mainline.\n" );

	char buf[64];
	if( !initPipes() )
		fail( "Child init failed.\n" );

	if( getEnvVar( "STEAMSHIM_RUNCLIENT", buf, sizeof( buf ) ) )
		GRunClient = true;
	if( getEnvVar( "STEAMSHIM_RUNSERVER", buf, sizeof( buf ) ) )
		GRunServer = true;

	if( !initSteamworks( GPipeWrite ) ) {
		char failure = 0;
		writePipe( GPipeWrite, &failure, sizeof failure );
		fail( "Failed to initialize Steamworks" );
	}

	char success = 1;
	writePipe( GPipeWrite, &success, sizeof success );

	dbgprintf( "Child in command processing loop.\n" );

	// Now, we block for instructions until the pipe fails (child closed it or
	//  terminated/crashed).
	processCommands();

	dbgprintf( "Child shutting down.\n" );

	// Close our ends of the pipes.
	closePipe( GPipeRead );
	closePipe( GPipeWrite );

	deinitSteamworks();

	return 0;
}

#ifdef _WIN32

// from win_sys
#define MAX_NUM_ARGVS 128
int argc;
char *argv[MAX_NUM_ARGVS];

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	if( strstr( GetCommandLineA(), "steamdebug" ) ) {
		debug = true;
		FreeConsole();
		AllocConsole();
		freopen( "CONOUT$", "w", stdout );
	}
	return main( argc, argv );
} // WinMain
#endif
