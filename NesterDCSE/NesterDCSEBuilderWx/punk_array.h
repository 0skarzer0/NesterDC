#ifndef _PUNK_ARRAY_H_
#define _PUNK_ARRAY_H_

#define DECLARE_PUNK_ARRAY(array_name, type)\
struct\
{\
	union\
	{\
		type* data;\
		void* vdata;\
	};\
	unsigned int count;\
	unsigned int bufcount;\
	unsigned int resize;\
} array_name;

#define punk_array_init(pa)\
	(pa).data = NULL;\
	(pa).count = 0;\
	(pa).bufcount = 0;\
	(pa).resize = 0x10;

#define punk_array_destroy(pa)\
	if ((pa).data)\
	{\
		free((pa).data);\
		(pa).data = NULL;\
	}

#define punk_array_push(pa, item)\
	if (!(pa).data)\
	{\
		(pa).bufcount = (pa).resize;\
		(pa).vdata = malloc((pa).bufcount * sizeof(*((pa).data)));\
	}\
\
	if ((pa).count == (pa).bufcount)\
	{\
		void* old = (pa).data;\
		(pa).bufcount += (pa).resize;\
		(pa).vdata = malloc((pa).bufcount * sizeof(*((pa).data)));\
		memcpy((pa).data, old, (pa).count * sizeof(*((pa).data)));\
		free(old);\
	}\
	(pa).data[(pa).count] = item;\
	++((pa).count);

#define punk_array_remove(pa, index)\
	{\
		unsigned int punk_array_k;\
		for (punk_array_k = index; punk_array_k < (pa).count; ++punk_array_k)\
			(pa).data[punk_array_k] = (pa).data[punk_array_k + 1];\
		--(pa).count;\
	}

#define punk_array_remove_all(pa)\
	(pa).count = 0;

#endif
