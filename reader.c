#include <node_api.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

FILE *file = NULL;
long size;

// Example: A function that seeks and reads file size (from your previous C example)
napi_value GetFileSize(napi_env env, napi_callback_info info) {
	napi_value result;
	napi_create_int64(env, size, &result);
	return result;
}

napi_value set(napi_env env, napi_callback_info info){
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	char filename[256];
	size_t filename_len;
	napi_get_value_string_utf8(env, argv[0], filename, 256, &filename_len);

	file = fopen(filename, "rb");
	if(file == NULL)
		return NULL;

	fseek(file, 0, SEEK_END);
	size = ftell(file);

	napi_value result;
	napi_status status = napi_create_int32(env, 0, &result);

	if (status != napi_ok) return NULL;
	return result;
}

napi_value get(napi_env env, napi_callback_info info){
	size_t argc = 2;          // We expect 2 arguments
	napi_value argv[2];       // Array to hold them
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	int32_t pos;
	napi_get_value_int32(env, argv[0], &pos);

	int32_t max;
	napi_get_value_int32(env, argv[1], &max);

	/*double percentage = ((double)(pos) / 1000.0);
	fseek(file, (long long) llround((percentage * (double)size)), SEEK_SET);

	// --- ADD THIS NUDGE ---
	if(percentage > 0 && percentage < 1.0){
		int c;
		// UTF-8 continuation bytes
		//start with bits '10xxxxxx' (0x80 to 0xBF)
		// We skip bytes until we
		//find a byte that ISN'T a continuation byte.
		while ((c = fgetc(file)) != EOF && (c & 0xC0) == 0x80);

		// We found the start of a real char, step back 1 so
		//the loop can read it.
		if (c != EOF) ungetc(c, file);
	}*/

	fseek(file, pos, SEEK_SET);

	char *out = malloc(max);
	memset(out, 0, max);

	char *opos = out;
	int cur = 0;
	while((cur = fgetc(file)) != EOF){
		if(opos - out >= max)
			break ;

		*opos = (char)cur;
		opos++;
	}

	*opos = '\0';

	//printf("PERCENTAGE: %d | SIZE: %l | RESULT: %l\n\n", percentage, size, (long)(percentage * size));
	/*printf("PERCENTAGE: %.2f\n", percentage);
	printf("SIZE: %ld\n", size);
	printf("POSITION: %lld\n",
		(long long) llround((percentage * (double)size))
	);*/

	//fclose(file);
	//printf("%i\n", strlen(out));

	napi_value result;
	napi_status status = napi_create_string_utf8(
		env,
		out,
		opos - out,
		&result
	);

	if(status != napi_ok) return NULL;
	return result;
}

napi_value Init(napi_env env, napi_value exports) {
	// Define all functions in an array
	napi_property_descriptor desc[] = {
		{ "getFileSize", 0, GetFileSize, 0, 0, 0, napi_default, 0 },
		{ "get", 0, get, 0, 0, 0, napi_default, 0 },
		{ "set", 0, set, 0, 0, 0, napi_default, 0 }
	};

	// Register the array (length is 2)
	napi_status status = napi_define_properties(env, exports, 3, desc);

	if (status != napi_ok) {
		napi_throw_error(env, NULL,
			"Failed to register native properties"
		);
	}

	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
