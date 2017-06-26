__kernel void bfs(__global int* map,
				 __global bool* mask,
				 __global const int *x_size,
				 __global const int *y_size,
				 __global const int *x_end,
				 __global const int *y_end,
				 __global int *dist,
				 __global bool *done,
				 __global bool *found){
	int x = get_global_id(0), y = get_global_id(1);
	int width = get_local_size(0);

	int idx = x + y * width;
	if(map[idx] == -3){
		map[idx] = *dist;

		if((x == *x_end && (y - *y_end == 1 || y - *y_end == -1)) || 
				((x - *x_end == 1 || x - *x_end == -1) && y == *y_end)){
			*found = true;
			map[*x_end + *y_end * width] = *dist + 1;
		}
		else{
			if(x != 0 && map[idx - 1] == -1){
				*done = false;
				mask[idx - 1] = true;
			}
			if(x != *x_size - 1 && map[idx + 1] == -1){
				*done = false;
				mask[idx + 1] = true;
			}
			if(y != 0 && map[idx - width] == -1){
				*done = false;
				mask[idx - width] = true;
			}
			if(y != *y_size - 1 && map[idx + width] == -1){
				*done = false;
				mask[idx + width] = true;
			}
		}
	}
}

__kernel void mask(__global int* map,
				 __global bool* mask){
	int x = get_global_id(0), y = get_global_id(1);
	int width = get_local_size(0);

	int idx = x + y * width;
	if(mask[idx] == true){
		map[idx] = -3;
		mask[idx] = false;
	}
}