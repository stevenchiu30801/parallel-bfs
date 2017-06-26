__kernel void bfs(__global int* map,
				 __global bool* mask,
				 __global const int *x_end,
				 __global const int *y_end,
				 __global int *dist,
				 __global bool *done,
				 __global bool *found){
	int x = get_global_id(0), y = get_global_id(1);
	int x_size = get_global_size(0);
	int y_size = get_global_size(1);

	int idx = x + y * x_size;
	if(map[idx] == -3){
		map[idx] = *dist;

		if((x == *x_end && (y - *y_end == 1 || y - *y_end == -1)) || 
				((x - *x_end == 1 || x - *x_end == -1) && y == *y_end)){
			*found = true;
			map[*x_end + *y_end * x_size] = *dist + 1;
		}
		else{
			if(x != 0 && map[idx - 1] == -1){
				*done = false;
				mask[idx - 1] = true;
			}
			if(x != x_size - 1 && map[idx + 1] == -1){
				*done = false;
				mask[idx + 1] = true;
			}
			if(y != 0 && map[idx - x_size] == -1){
				*done = false;
				mask[idx - x_size] = true;
			}
			if(y != y_size - 1 && map[idx + x_size] == -1){
				*done = false;
				mask[idx + x_size] = true;
			}
		}
	}
}

__kernel void mask(__global int* map,
				 __global bool* mask){
	int x = get_global_id(0), y = get_global_id(1);
	int x_size = get_global_size(0);

	int idx = x + y * x_size;
	if(mask[idx] == true){
		map[idx] = -3;
		mask[idx] = false;
	}
}