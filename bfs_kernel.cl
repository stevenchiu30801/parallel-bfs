__kernel void bfs(__global int* map,
				 __global const int *x_size,
				 __global const int *y_size,
				 __global const int *x_end,
				 __global const int *y_end,
				 __global int *dist,
				 __global bool *done,
				 __global bool *found){
	int x = get_global_id(0), y = get_global_id(1);
	int width = get_local_size(0);

	if(map[x + y * width] == -3){
		map[x + y * width] = *dist;

		if((x == *x_end && (y - *y_end == 1 || y - *y_end == -1)) || 
				((x - *x_end == 1 || x - *x_end == -1) && y == *y_end)){
			*found = true;
			map[*x_end + *y_end * width] = *dist + 1;
		}
		else{
			if(x != 0 && map[(x - 1) + y * width] == -1){
				*done = false;
				map[(x - 1) + y * width] = -4;
			}
			if(x != *x_size - 1 && map[(x + 1) + y * width] == -1){
				*done = false;
				map[(x + 1) + y * width] = -4;
			}
			if(y != 0 && map[x + (y - 1) * width] == -1){
				*done = false;
				map[x + (y - 1) * width] = -4;
			}
			if(y != *y_size - 1 && map[x + (y + 1) * width] == -1){
				*done = false;
				map[x + (y + 1) * width] = -4;
			}
		}
	}
}

__kernel void mask(__global int* map){
	int x = get_global_id(0), y = get_global_id(1);
	int width = get_local_size(0);

	if(map[x + y * width] == -4)
		map[x + y * width] = -3;
}