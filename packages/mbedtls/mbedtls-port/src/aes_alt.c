#include "mbedtls/aes.h"

#ifdef MBEDTLS_AES_ALT
#include <string.h>
#include <uart_pub.h>
#include <hal_aes.h>

/**
 * number of active contexts, used for power on/off of the crypto core
 */
static unsigned int sRefNum = 0;
static uint8_t current_key[32];
static size_t current_len = 0;

/*
 * Documented in aes_alt.h
 */
void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    memset((void *)ctx, 0x00, sizeof(ctx));
    if (sRefNum++ == 0)
    {
        hal_aes_init(ctx->hal_ctx);
    }
}

/*
 * Documented in aes_alt.h
 */
void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (--sRefNum == 0)
    {
        //hal_aes_free(ctx->hal_ctx);
        current_len = 0;
    }

    memset((void *)ctx, 0x00, sizeof(ctx));

    return;
}

/*
 * Documented in aes_alt.h
 */
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    if ((128 != keybits) && (192 != keybits) && (256 != keybits))
    {
        bk_printf("[HAL_SEC]%d keybits(%d) is invalid\n", __LINE__, keybits);
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ctx->key_len = keybits/8;
    memcpy(ctx->key, key, ctx->key_len);

    return 0;
}

/*
 * Documented in aes_alt.h
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    if ((128 != keybits) && (192 != keybits) && (256 != keybits))
    {
        bk_printf("[HAL_SEC]%d keybits(%d) is invalid\n", __LINE__, keybits);
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ctx->key_len = keybits/8;
    memcpy(ctx->key, key, ctx->key_len);

    return 0;
}

/*
 * Documented in aes_alt.h
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
    int retval;
    int hal_mode;
    int (*hal_aes_setkey_proc)(void *, const unsigned char *, unsigned int );

    if (MBEDTLS_AES_ENCRYPT == mode)
    {
        hal_mode = ENCODE;
        hal_aes_setkey_proc = hal_aes_setkey_enc;
    }
    else
    {
        hal_mode = DECODE;
        hal_aes_setkey_proc = hal_aes_setkey_dec;
    }

    if ((current_len != ctx->key_len) || (0 != memcmp(current_key, ctx->key, current_len)))
    {
        hal_aes_setkey_proc(ctx->hal_ctx, ctx->key, ctx->key_len * 8);
        current_len = ctx->key_len;
        memcpy(current_key, ctx->key, current_len);
    }

    retval = hal_aes_crypt_ecb(ctx->hal_ctx, hal_mode, input, output);
    if (0 != retval)
    {
        bk_printf("[HAL_SEC]%d failed(%d)\n", __LINE__, retval);
    }

    return retval;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    int i;
    unsigned char temp[16];

    if( length % 16 )
    {
        bk_printf("[HAL_SEC]%d length(%d) is invalid\n", __LINE__, length);
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length > 0 )
        {
            memcpy( temp, input, 16 );
            mbedtls_aes_crypt_ecb( ctx, mode, input, output );

            for( i = 0; i < 16; i++ )
                output[i] = (unsigned char)( output[i] ^ iv[i] );

            memcpy( iv, temp, 16 );

            input  += 16;
            output += 16;
            length -= 16;
        }
    }
    else
    {
        while( length > 0 )
        {
            for( i = 0; i < 16; i++ )
                output[i] = (unsigned char)( input[i] ^ iv[i] );

            mbedtls_aes_crypt_ecb( ctx, mode, output, output );
            memcpy( iv, output, 16 );

            input  += 16;
            output += 16;
            length -= 16;
        }
    }

    return( 0 );
}

#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c;
    size_t n = *iv_off;

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

    return( 0 );
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c, i;
    size_t n = *nc_off;

    while( length-- )
    {
        if( n == 0 ) {
            mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block );

            for( i = 16; i > 0; i-- )
                if( ++nonce_counter[i - 1] != 0 )
                    break;
        }
        c = *input++;
        *output++ = (unsigned char)( c ^ stream_block[n] );

        n = ( n + 1 ) & 0x0F;
    }

    *nc_off = n;

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */
