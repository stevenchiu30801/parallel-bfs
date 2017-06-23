#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <CL/cl.h>
#include <vector>
#include <stack>
#define MAX_LINE 25

using namespace std;

class Node{
public:
	int x, y;	// x, y coordinate
	Node(int x, int y);
	Node(const Node &c);
};

Node::Node(int x, int y){
	this->x = x;
	this->y = y;
}

Node::Node(const Node &c){
	this->x = c.x;
	this->y = c.y;
}

cl_mem _clCreateBuf(cl_context context, cl_mem_flags flags, int size, void* h_mem_ptr){
	cl_int err;
	cl_mem d_mem = clCreateBuffer(context, flags, size, h_mem_ptr, &err);
	if(err != CL_SUCCESS){
		cerr << "Unable to create buffer\n";
		if(err == CL_INVALID_CONTEXT)
			cerr << "CL_INVALID_CONTEXT\n";
		else if(err == CL_INVALID_VALUE)
			cerr << "CL_INVALID_VALUE\n";
		else if(err == CL_INVALID_BUFFER_SIZE)
			cerr << "CL_INVALID_BUFFER_SIZE\n";
		else if(err == CL_INVALID_HOST_PTR)
			cerr << "CL_INVALID_HOST_PTR\n";
		else if(err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
			cerr << "CL_MEM_OBJECT_ALLOCATION_FAILURE\n";

		exit(1);
	}

	return d_mem;
}

cl_mem _clCreateImg2D(cl_context context, cl_mem_flags flags, int width, int height, int row_pitch, void *h_mem_ptr){
	cl_int err;
	cl_image_format imgformat;
	imgformat.image_channel_order = CL_RGBA;
	imgformat.image_channel_data_type = CL_SNORM_INT8;
	cl_mem d_mem = clCreateImage2D(context, flags, &imgformat, width, height, row_pitch, h_mem_ptr, &err);
	if(err != CL_SUCCESS){
		cerr << "Unable to create image\n";
		if(err == CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			cerr << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n";
		else if(err == CL_INVALID_IMAGE_SIZE)
			cerr << "CL_INVALID_IMAGE_SIZE\n";
		else if(err == CL_INVALID_HOST_PTR)
			cerr << "CL_INVALID_HOST_PTR\n";
		else if(err == CL_IMAGE_FORMAT_NOT_SUPPORTED)
			cerr << "CL_IMAGE_FORMAT_NOT_SUPPORTED\n";
		else if(err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
			cerr << "CL_MEM_OBJECT_ALLOCATION_FAILURE\n";
		exit(1);
	}

	return d_mem;
}

void _clMemcpyH2D(cl_command_queue queue, cl_mem d_mem, int size, const void *h_mem_ptr){
	cl_int err;
	err = clEnqueueWriteBuffer(queue, d_mem, CL_TRUE, 0, size, h_mem_ptr, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		cerr << "Unable to write buffer\n";
		if(err == CL_INVALID_COMMAND_QUEUE)
			cerr << "CL_INVALID_COMMAND_QUEUE\n";
		else if(err == CL_INVALID_CONTEXT)
			cerr << "CL_INVALID_CONTEXT\n";
		else if(err == CL_INVALID_MEM_OBJECT)
			cerr << "CL_INVALID_MEM_OBJECT\n";
		else if(err == CL_INVALID_VALUE)
			cerr << "CL_INVALID_VALUE\n";
		else if(err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
			cerr << "CL_MEM_OBJECT_ALLOCATION_FAILURE\n";

		exit(1);
	}
}

void _clMemcpyD2H(cl_command_queue queue, cl_mem d_mem, int size, void *h_mem_ptr){
	cl_int err;
	err = clEnqueueReadBuffer(queue, d_mem, CL_TRUE, 0, size, h_mem_ptr, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		cerr << "Unable to read buffer\n";
		exit(1);
	}
}

cl_program load_program(cl_context context, const char* clfile){
	ifstream fin(clfile, ios::in | ios::binary);
	if(!fin.good()){
		cerr << "Unable to open file\n";
		return NULL;
	}

	/***** get file length *****/
	fin.seekg(0, ios::end);
	size_t length = fin.tellg();
	fin.seekg(0);

	/***** read program source *****/
	vector<char> data(length + 1);
	fin.read(&data[0], length);
	data[length] = '\0';

	/***** create and build program *****/
	const char* source = &data[0];
	cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
	if(program == NULL){
		cerr << "Unable to create program\n";
		return NULL;
	}

	cl_int err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(err != CL_SUCCESS){
		cerr << "Unable to build program\n";
		if(err == CL_INVALID_PROGRAM)
			cerr << "CL_INVALID_PROGRAM\n";
		else if(err == CL_INVALID_VALUE)
			cerr << "CL_INVALID_VALUE\n";
		else if(err == CL_INVALID_DEVICE)
			cerr << "CL_INVALID_DEVICE\n";
		else if(err == CL_INVALID_BINARY)
			cerr << "CL_INVALID_BINARY\n";
		else if(err == CL_INVALID_BUILD_OPTIONS)
			cerr << "CL_INVALID_BUILD_OPTIONS\n";
		else if(err == CL_INVALID_OPERATION)
			cerr << "CL_INVALID_OPERATION\n";
		else if(err == CL_COMPILER_NOT_AVAILABLE)
			cerr << "CL_COMPILER_NOT_AVAILABLE\n";
		else if(err == CL_BUILD_PROGRAM_FAILURE)
			cerr << "CL_BUILD_PROGRAM_FAILURE\n";
		return NULL;
	}

	return program;
}

int main(int argc, char **argv){
	if(argc != 2){
		cerr << "Incorrect input. Usage: " << argv[0] << " <map>\n";
		exit(1);
	}

	int x_size, y_size;	// map size
	int x_start, y_start, x_end, y_end;	// start and end point
	int **map;

	char buf[MAX_LINE];
	char *pch;

	/***** reading file *****/
	fstream file;
	file.open(argv[1], ios::in);

	/***** map initialization *****/
	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_size = atoi(pch);
	pch = strtok(NULL, " ");
	y_size = atoi(pch);

	x_size++;
	y_size++;

	map = new int*[x_size];
	for(int i = 0; i < x_size; i++){
		map[i] = new int[y_size];
		for(int j = 0; j < y_size; j++)
			map[i][j] = -1;
	}

	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_start = atoi(pch);
	pch = strtok(NULL, " ");
	y_start = atoi(pch);

	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_end = atoi(pch);
	pch = strtok(NULL, " ");
	y_end = atoi(pch);

	/***** blockages initialization *****/
	while(file.getline(buf, MAX_LINE, '\n') != NULL){
		int x, y, _x, _y;
		pch = strtok(buf, " ");
		x = atoi(pch);
		pch = strtok(NULL, " ");
		y = atoi(pch);
		pch = strtok(NULL, " ");
		_x = atoi(pch);
		pch = strtok(NULL, " ");
		_y = atoi(pch);

		if(x <= _x && y <= _y)
			for(int i = x; i <= _x; i++)
				for(int j = y; j <= _y; j++)
					map[i][j] = -2;
		else if(x <= _x && y >= _y)
			for(int i = x; i <= _x; i++)
				for(int j = y; j >= _y; j--)
					map[i][j] = -2;
		else if(x >= _x && y <= _y)
			for(int i = _x; i >= x; i--)
				for(int j = y; j <= _y; j++)
					map[i][j] = -2;
		else
			for(int i = _x; i >= x; i--)
				for(int j = _y; j >= y; j--)
					map[i][j] = -2;
	}

	file.close();
	/***** end of reading file and initializing *****/

	/***** OpenCL environment setting *****/
	/***** get available platforms *****/
	cl_int err;	// error code returned from OpenCL calls
	cl_uint num;	// number of available platforms
	err = clGetPlatformIDs(0, 0, &num);
	if(err != CL_SUCCESS){
		cerr << "Unable to get platforms\n";
		exit(1);
	}

	vector<cl_platform_id> platforms(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if(err != CL_SUCCESS){
		cerr << "Unable to get platform ID\n";
		exit(1);
	}

	/***** create a context *****/
	cl_context_properties prop[] = {CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0};
	cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, NULL);
	if(context == 0) {
		cerr << "Can't create OpenCL context\n";
		exit(1);
	}

	/***** get context devices *****/
	size_t cb;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);

	vector<cl_device_id> devices(cb / sizeof(cl_device_id));
	clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, &devices[0], 0);

	/***** get device info, if needed *****/
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
	string devname;
	devname.resize(cb);
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, &devname[0], 0);

	cl_ulong devbufsize;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &devbufsize, 0);
	// cout << "valid buffer max size " << devbufsize << endl;

	cl_ulong imgwidthsize;
	clGetDeviceInfo(devices[0], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(cl_ulong), &imgwidthsize, 0);
	// cout << "valid image width max size " << imgwidthsize << endl;

	/***** create command queue *****/
	cl_command_queue queue = clCreateCommandQueue(context, devices[0], 0, 0);
	if(queue == 0){
		cerr << "Cannot create command queue\n";
		clReleaseContext(context);
		exit(1);
	}

	/***** create buffer/image on device *****/
	/*	map[][]
	 *	x_size, y_size
	 *	x_end, y_end
	 *	dist
	 *	done
	 */
	map[x_start][y_start] = -3;	// add mask to start point
	cl_mem cl_map = _clCreateImg2D(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, x_size, y_size, 0, &map[0][0]);
	cl_mem cl_x_size = _clCreateBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &x_size);
	cl_mem cl_y_size = _clCreateBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &y_size);
	cl_mem cl_x_end = _clCreateBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &x_end);
	cl_mem cl_y_end = _clCreateBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &y_end);
	cl_mem cl_dist = _clCreateBuf(context, CL_MEM_READ_ONLY, sizeof(int), NULL);
	cl_mem cl_done = _clCreateBuf(context, CL_MEM_WRITE_ONLY, sizeof(bool), NULL);
	cl_mem cl_found = _clCreateBuf(context, CL_MEM_WRITE_ONLY, sizeof(bool), NULL);

	/***** write buffer from host memory *****/
	// _clMemcpyH2D(queue, cl_x_size, sizeof(int), &x_size);
	// _clMemcpyH2D(queue, cl_y_size, sizeof(int), &y_size);
	// _clMemcpyH2D(queue, cl_x_end, sizeof(int), &x_end);
	// _clMemcpyH2D(queue, cl_y_end, sizeof(int), &y_end);

	/***** load kernel file to memory *****/
	cl_program program = load_program(context, "bfs_kernel.cl");
	if(program == NULL){
		clReleaseMemObject(cl_map);
		clReleaseMemObject(cl_x_size);
		clReleaseMemObject(cl_y_size);
		clReleaseMemObject(cl_x_end);
		clReleaseMemObject(cl_y_end);
		clReleaseMemObject(cl_dist);
		clReleaseMemObject(cl_done);
		clReleaseMemObject(cl_found);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		exit(1);
	}

	/***** create kernel object *****/
	cl_kernel kernel = clCreateKernel(program, "bfs", 0);	// "bfs" is the function name in "bfs_kernel.cl"
	if(kernel == NULL){
		clReleaseProgram(program);
		clReleaseMemObject(cl_map);
		clReleaseMemObject(cl_x_size);
		clReleaseMemObject(cl_y_size);
		clReleaseMemObject(cl_x_end);
		clReleaseMemObject(cl_y_end);
		clReleaseMemObject(cl_dist);
		clReleaseMemObject(cl_done);
		clReleaseMemObject(cl_found);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		exit(1);
	}
	/***** end of OpenCL environment setting *****/

	cout << "OpenCL setting done\n";

	/***** breadth-first search *****/
	/*	map value notation
	 *	-3 : mask, points to be traversed next stage
	 *	-2 : blockages
	 *	-1 : untraversed points
	 *	>= 0 : distance from start point
	 */
	int dist = -1;
	_clMemcpyH2D(queue, cl_dist, sizeof(int), &dist);
	bool found = false;	// true if end point is found
	_clMemcpyH2D(queue, cl_found, sizeof(bool), &found);
	bool done;

	do{
		done = true;	// true if bfs is done, i.e. all accessible points are traversed
		_clMemcpyH2D(queue, cl_done, sizeof(bool), &done);
		dist++;	// distance from start point within this stage
		_clMemcpyH2D(queue, cl_dist, sizeof(int), &dist);

		/***** set kernel argument *****/
		int arg_idx = 0;
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_map);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_x_size);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_y_size);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_x_end);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_y_end);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_dist);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_done);
		clSetKernelArg(kernel, arg_idx++, sizeof(cl_mem), &cl_found);

		/***** enqueue command to execute kernel *****/
		size_t work_size[2] = {(size_t)x_size, (size_t)y_size};
		err = clEnqueueNDRangeKernel(queue, kernel, 2, 0, work_size, NULL, 0, NULL, NULL);
		if(err != CL_SUCCESS){
			clReleaseKernel(kernel);
			clReleaseProgram(program);
			clReleaseMemObject(cl_map);
			clReleaseMemObject(cl_x_size);
			clReleaseMemObject(cl_y_size);
			clReleaseMemObject(cl_x_end);
			clReleaseMemObject(cl_y_end);
			clReleaseMemObject(cl_dist);
			clReleaseMemObject(cl_done);
			clReleaseMemObject(cl_found);
			clReleaseCommandQueue(queue);
			clReleaseContext(context);
			exit(1);
		}

		_clMemcpyD2H(queue, cl_done, sizeof(bool), &done);
		_clMemcpyD2H(queue, cl_found, sizeof(bool), &found);
	}while(done == false && found == false);

	/***** end of breadth-first search *****/

	/***** output file *****/
	_clMemcpyD2H(queue, cl_map, sizeof(int) * x_size * y_size, &map);

	if(map[x_end][y_end] == -1){	// no path from start point to end point
		// cout << "There is no path from (" << x_start << ", " << y_start
		// 	<< ") to (" << x_end << ", " << y_end << ")\n";

		char *outfile = new char[strlen(argv[1] + 6)];
		sprintf(outfile, "path_%s", argv[1]);
		fstream fout;
		fout.open(outfile, ios::out);

		strcpy(buf, "No Path");
		fout.write(buf, strlen(buf));

		fout.close();
	}
	else{
		// cout << "Shortest distance from (" << x_start << ", " << y_start
		// 	<< ") to (" << x_end << ", " << y_end << ") : " << map[x_end][y_end] << endl;

		/***** backtrack the path *****/
		stack<Node> path;
		int dist = map[x_end][y_end] - 1;
		Node u(x_end, y_end);
		path.push(u);

		while(dist >= 0){
			int xtmp = u.x, ytmp = u.y;
			if(xtmp != 0 && map[xtmp - 1][ytmp] == dist){
				u.x--;
				path.push(u);
			}
			else if(xtmp != x_size - 1 && map[xtmp + 1][ytmp] == dist){
				u.x++;
				path.push(u);
			}
			else if(ytmp != 0 && map[xtmp][ytmp - 1] == dist){
				u.y--;
				path.push(u);
			}
			else{
				u.y++;
				path.push(u);
			}

			dist--;
		}

		/***** output shortest distance and path to file *****/
		char *outfile = new char[strlen(argv[1] + 6)];
		sprintf(outfile, "path_%s", argv[1]);
		fstream fout;
		fout.open(outfile, ios::out);

		sprintf(buf, "%d\n", map[x_end][y_end]);
		fout.write(buf, strlen(buf));

		for(int i = path.size(); i > 0; i--){
			sprintf(buf, "%d %d\n", path.top().x, path.top().y);
			fout.write(buf, strlen(buf));

			path.pop();
		}

		fout.close();

		delete [] outfile;
	}
	/***** end of output file *****/

	/***** deallocate memory *****/
	for(int i = 0; i < x_size; i++)
		delete [] map[i];
	delete [] map;

	/***** release OpenCL resources *****/
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(cl_map);
	clReleaseMemObject(cl_x_size);
	clReleaseMemObject(cl_y_size);
	clReleaseMemObject(cl_x_end);
	clReleaseMemObject(cl_y_end);
	clReleaseMemObject(cl_dist);
	clReleaseMemObject(cl_done);
	clReleaseMemObject(cl_found);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;
}
