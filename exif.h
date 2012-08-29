#ifndef EXIF_H
#define EXIF_H

#include <iostream>
#include <algorithm>

class exif_data
{
	string file_name;
	string camera_model;
	string date_time;
public:
	exif_data(char* filename): file_name(filename) {}
	void set_model(char* data) { camera_model= data; }
	void set_date_time(char* data) { date_time = data; }
	void dump()
	{
		cout << "Camera model is: " << camera_model <<'\n';
		cout << "Photo was taken on: " << date_time << '\n';
	}
	void rename_file()
	{
		string new_file_name(date_time+" "+camera_model+" "+file_name);
		std::replace( new_file_name.begin(), new_file_name.end(), ':', '-');
		rename ( file_name.c_str(),  new_file_name.c_str() );

	}
};

#endif  // EXIF_H
