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

	gpro-net-Client-Plugin.cpp
	Main implementation for plugin; instantiate and manage networking.
*/

#include "gpro-net-Client-Plugin.h"

// utility
const int MAX_SHORT_VAL = 65536;
const int MAX_CHAR_VAL = 128;
// rotation
const float MAX_DEGREE = 360.0f;
// scale
const float MAX_SCALE = 1000.0f;
// translation
const float TRANSLATION_BOUNDS = 1000.0f;
const float MAX_TRANSLATION = TRANSLATION_BOUNDS * 2.0f;


struct sSpatialPose
{
	int id;
	short scale[3];     // non-uniform scale (0, 1000]
	short rotate[3];    // orientation as Euler angles [0, 360)
	short translate[3]; // translation [-1000, 1000]

	/*
	char updates[9]; // hold which components that were just updated

	// only write the components that have been changed to send
	RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
	{
		bitstream.Write(updates[0]);
		bitstream.Write(updates[1]);
		bitstream.Write(updates[2]);
		bitstream.Write(updates[3]);
		bitstream.Write(updates[4]);
		bitstream.Write(updates[5]);
		bitstream.Write(updates[6]);
		bitstream.Write(updates[7]);
		bitstream.Write(updates[8]);

		if(scale[0] updated)
			bitstream.Write(scale[0]);
		if(scale[1] updated)
			bitstream.Write(scale[1]);
		if(scale[2] updated)
			bitstream.Write(scale[2]);
		
		// and so on:
		if(rotate updated)
		{
			bitstream.Write(rotate[0]);
			bitstream.Write(rotate[1]);
			bitstream.Write(rotate[2]);
		}
		if(translate updated)
		{
			bitstream.Write(translate[0]);
			bitstream.Write(translate[1]);
			bitstream.Write(translate[2]);
		}
		return bitstream;
	}

	// read from stream
	RakNet::BitStream& Read(RakNet::BitStream& bitstream)
	{
		bitstream.Read(updatedComponents);

		if(updatedComponents[0])
			bitstream.Read(scale[0]);

		...

		if(updatedComponents[3])
			bitstream.Read(rotate[0]);

		...

		if(updatedComponents[6])
			bitstream.Read(translate[0]);

		...

		return bitstream;
	}

	*/

} agentTransformData; // <- would replace this with finding the specific object you are handling

// Compress unity transform data for sending
void setScale(float x, float y, float z)
{
	agentTransformData.scale[0] = (short)((x / MAX_SCALE) * MAX_SHORT_VAL);
	agentTransformData.scale[1] = (short)((y / MAX_SCALE) * MAX_SHORT_VAL);
	agentTransformData.scale[2] = (short)((z / MAX_SCALE) * MAX_SHORT_VAL);
}

void setRot(float x, float y, float z)
{
	agentTransformData.rotate[0] = (short)((x / MAX_DEGREE) * MAX_SHORT_VAL);
	agentTransformData.rotate[1] = (short)((y / MAX_DEGREE) * MAX_SHORT_VAL);
	agentTransformData.rotate[2] = (short)((z / MAX_DEGREE) * MAX_SHORT_VAL);
}

void setTrans(float x, float y, float z)
{
	agentTransformData.rotate[0] = (short)(((x+TRANSLATION_BOUNDS) / MAX_TRANSLATION) * MAX_SHORT_VAL);
	agentTransformData.rotate[1] = (short)(((y+TRANSLATION_BOUNDS) / MAX_TRANSLATION) * MAX_SHORT_VAL);
	agentTransformData.rotate[2] = (short)(((z+TRANSLATION_BOUNDS) / MAX_TRANSLATION) * MAX_SHORT_VAL);
}

// De-compress sent data for unity
//  Would probalby exist on unity side
float getScaleComponent(int index, short compressedValue)
{
	return (float)agentTransformData.scale[index] / (float)MAX_SHORT_VAL * MAX_SCALE;
}

float getRotComponent(int index, short compressedValue)
{
	return (float)agentTransformData.rotate[index] / (float)MAX_SHORT_VAL * MAX_DEGREE;
}

float getTransComponent(int index, short compressedValue)
{
	return (float)agentTransformData.scale[index] / (float)MAX_SHORT_VAL * MAX_TRANSLATION - TRANSLATION_BOUNDS;
}

// -- handle error -- //
// s is the 'tru' position recieved from the server
float handleError(sSpatialPose s)
{
	// you know 'true' position recieved is >=1 out of date
	// decompress the s' values using functions above
	// any values that weren't sent, you'd fill with the values you already have
	float uncompressedScale[3]; // <- probably irrelevant to below calcs 99% of time
	float uncompressedRotate[3];
	float uncompressedTranslate[3];

	// preform dead reckoning estimate over amount of frames
	// where [time since updated] is how old the new information is
	//   unompressedRotate += angVel * [time since updated] + angAcc * [time since updated]^2
	//   uncompressedTranslate += vel * [time since updated] + acc * [time since updated]^2

	// Set teh unity-side values to the result of those calculations
}