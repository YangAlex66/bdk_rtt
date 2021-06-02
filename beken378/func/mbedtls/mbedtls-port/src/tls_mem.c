#include "tls_config.h"

#ifdef MBEDTLS_PLATFORM_MEMORY
#include "mem_pub.h"

void *tls_mbedtls_mem_calloc(size_t n, size_t size)
{
	unsigned int len = n * size;
	if(len == 0){
		return 0;
	}
    return os_zalloc( len );
}

void tls_mbedtls_mem_free(void *ptr)
{
    os_free(ptr);
}

#endif /* !MBEDTLS_PLATFORM_MEMORY */
