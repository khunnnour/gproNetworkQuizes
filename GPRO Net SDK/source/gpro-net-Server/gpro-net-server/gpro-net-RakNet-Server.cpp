/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet-Server.cpp
	Source for RakNet server management.
*/

#include "gpro-net/gpro-net-server/gpro-net-RakNet-Server.hpp"
#include <string>
#include <vector>

class client {
public:
	RakNet::RakString username;
	RakNet::SystemAddress sysAddress;
};

const int GPRO_MAX_PLAYERS = 5;

class GameServer {
public:
	int serverID;
	int currNumPlayers;
	std::vector<client> inGamePlayers;

	GameServer(int sID) {
		serverID = sID;
		currNumPlayers = 0;
	}

	void PlayerJoin(client p)
	{
		inGamePlayers.push_back(p);
		currNumPlayers++;
	}
	void PlayerQuit(client p)
	{
		// find the client who left and remove from vector
		currNumPlayers--;
	}

	// overload operator to rak string
};

namespace gproNet
{
	cRakNetServer::cRakNetServer()
	{
		RakNet::SocketDescriptor sd(SET_GPRO_SERVER_PORT, 0);
		unsigned short MAX_CLIENTS = 10;

		peer->Startup(MAX_CLIENTS, &sd, 1);
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);

		// creates all of the game rooms
		for (int i = 0; i < GPRO_MAX_NUM_GAMES; i++)
		{
			allGames[i] = GameServer(i);
		}
	}

	cRakNetServer::~cRakNetServer()
	{
		peer->Shutdown(0);
	}

	bool cRakNetServer::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		if (cRakNetManager::ProcessMessage(bitstream, sender, dtSendToReceive, msgID))
			return true;

		// server-specific messages
		switch (msgID)
		{
		case ID_NEW_INCOMING_CONNECTION:
			// *- Prompt for login or something at least to get username -* //
			// Send "login request" message
			//printf("A connection is incoming.\n");
			return true;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			//printf("The server is full.\n");
			return true;
		case ID_DISCONNECTION_NOTIFICATION:
			//printf("A client has disconnected.\n");
			return true;
		case ID_CONNECTION_LOST:
			//printf("A client lost the connection.\n");
			return true;

			// test message
		case ID_GPRO_MESSAGE_COMMON_BEGIN:
		{
			// server receives login, verify it
			RakNet::BitStream bitstream_w;
			
			RakNet::RakString user;
			bitstream_w.Read(user);

			// Verify the user is good
			/*
			if(InList(allClients, username) != -1)
			{	
				// Throw error
				return false;
			}
			*/
			//ReadTest(bitstream);
			client newClient;
			newClient.username = user;
			newClient.sysAddress = sender;
			// add to clients vector

			WriteTest(bitstream_w, "Hello client from server");
			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}	return true;
		case ID_GPRO_REQUEST_GAMES:
		{
			// Send a list of all active games
			RakNet::BitStream bitstream_w;

			WriteTimestamp(bitstream_w);
			bitstream.Write((RakNet::MessageID)ID_GPRO_REQUEST_GAMES);
			bitstream.Write(GetActiveGames());

			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}	return true;
		case ID_GPRO_REQUEST_JOIN_GAME:
		{
			// Checks if valid game provided
			/*
			if(0<game<MAX)
			{
				games[game].PlayerJoin();
			}
			*/
			// Moves them to that room
		}	return true;
		case ID_GPRO_LEAVE_GAME:
		{
			// Check if they are even
			// Checks if valid game provided
			/*
			if(0<game<MAX)
			{
				games[game].PlayerQuit();
			}
			*/
		}	return true;
		}
		return false;
	}

	RakNet::RakString cRakNetServer::GetActiveGames()
	{
		RakNet::RakString allGs="All Games:\n";

		for (int i = 0; i < GPRO_MAX_NUM_GAMES; i++)
		{
			allGs += allGames[i].serverID;
			allGs += " - ";
			allGs += allGames[i].currNumPlayers;
			allGs += " / ";
			allGs += GPRO_MAX_PLAYERS;
			allGs += "\n";
		}

		return allGs;
	}
}