#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
using namespace std;

#include "defs.h"
#include "utils.h"
#include "exif.h"

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "No file name given\n";
		return 0;
	}

	bool conversionNeeded = false;
	stringstream debugstream;

	ifstream file;
	file.open(argv[1], ios::binary);
	if ( file.fail() )
	{
		cout << "Can't open file!\n";
		return 0;
	}

	file.seekg (0, ios::end);
	unsigned int file_size = file.tellg();
	file.seekg (0, ios::beg);

	unsigned char *jpeg = new unsigned char[file_size];
	file.read((char*)jpeg, file_size);
	file.close();

	exif_data ex(argv[1]);

	if ( !(jpeg[0] == 0xFF && jpeg[1] == 0xD8 && jpeg[file_size-2] == 0xFF && jpeg[file_size-1] == 0xD9) )
	{
		cout << "Not a JPEG file\n";
		return 0;
	}
	file.seekg (0, ios::beg);
	DEBUG ("File size is: " << dec << file_size << " (" << (float)file_size/1048576 <<" MB)");
	unsigned int i;
	for (i=0; i<file_size-1; ++i)
		if (jpeg[i] == 0xFF && jpeg[i+1] == 0xE1)
		{
			i += 2;
			unsigned int app1_start = i;

			DEBUG ( "Found APP1 marker at " << i )
			unsigned short app1_data_size = jpeg[i] << 8 | jpeg[i+1];
			DEBUG ( "APP1 data size: " << app1_data_size )
			/* First 2 bytes after APP1 marker represent the data size including these 2 bytes */
			unsigned short info_size = app1_data_size - 2;
			/* Skip data size and Exif Header (6 bytes) */
			i += 8;
			info_size -= 8;
			/* Next 8 bytes are the TIFF header */
			unsigned int tiff_start = i;
			DEBUG ( "TIFF header starts at: " << tiff_start )
			i += 2;
			bool littleEndian = false;
			if (jpeg[i-2]=='I'&&jpeg[i-1]=='I') littleEndian = true;
			if (jpeg[i-2]=='M'&&jpeg[i-1]=='M' && isLittleEndian())
				conversionNeeded = true;
			i += 2; // these 2 bytes should be 0x2a00 for Intel, 0x002a for Motorola
			info_size -= 4;

			/* The last 4 bytes are the offset to the first Image File Directory
			starting from the TIFF header start*/
			int32 first_IFD_offset;
			SET_INT32(first_IFD_offset, jpeg[i])
			DEBUG ( "First IFD offset: " << first_IFD_offset )
			i -= 4;
			info_size += 4;
			bool foundModel = false;
			bool foundDateTime = false;
			for (int j=0; j<info_size; ++j)
			{

				if ( foundModel == false && (( littleEndian && jpeg[i+j]==0x10 && jpeg[i+j+1]==0x01) || (!littleEndian && jpeg[i+j]==0x01 && jpeg[i+j+1]==0x10) ))
				{
				DEBUG ( "Found camera model tag at: " << i+j )
				int32 comp_nr;
				SET_INT32(comp_nr, jpeg[i+j+4])
				DEBUG ( "Nr of components / Camera name contains: " << comp_nr << " characters" )
				if (comp_nr >4 )
				{
					int32 model_offset;
					SET_INT32 (model_offset, jpeg[i+j]+8);
					DEBUG ( "Camera model offset is: " << model_offset )
					ex.set_model((char*)&jpeg[tiff_start+model_offset]);
					foundModel = true;
				}
				else
					{
						ex.set_model((char*)&jpeg[i+j+8]);
						foundModel=true;
					}
				}

				if ( foundDateTime == false && ( (littleEndian && jpeg[i+j]==0x32 && jpeg[i+j+1]==0x01) || (!littleEndian && jpeg[i+j]==0x01 && jpeg[i+j+1]==0x32) ) )
				{
				DEBUG ( "Found DateTime tag at: " << i+j )
				int32 datetime_offset;
				SET_INT32 (datetime_offset, jpeg[i+j]+8);
				DEBUG ( "DateTime offset is: " <<  datetime_offset )
				ex.set_date_time((char*)&jpeg[tiff_start+datetime_offset]);
				foundDateTime = true;
				}
			}
			break;
		}

#ifdef VERBOSE
	ex.dump();
#endif
	ex.rename_file();
	delete [] jpeg;
	return 0;
}
