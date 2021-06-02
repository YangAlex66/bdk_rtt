#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef MBEDTLS_AES_ALT

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t key[32];
    size_t key_len;
    void *hal_ctx;
} mbedtls_aes_context;

/**
 * @brief Initialize AES context
 *
 * @param [in,out] ctx  AES context to be initialized
 */
void mbedtls_aes_init(mbedtls_aes_context *ctx);

/**
 * @brief          Clear AES context
 *
 * @param [in,out] ctx  AES context to be cleared
 */
void mbedtls_aes_free(mbedtls_aes_context *ctx);

/**
 * @brief          AES key schedule (encryption)
 *
 * @param [in,out] ctx      AES context to be used
 * @param [in]     key      Encryption key
 * @param [in]     keybits  Must be 128, 192 or 256
 *
 * @retval 0                                   If successful
 * @retval MBEDTLS_ERR_AES_INVALID_KEY_LENGTH
 */
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits);

/**
 * @brief          AES key schedule (decryption)
 *
 * @param [in,out] ctx      AES context to be used
 * @param [in]     key      Decryption key
 * @param [in]     keybits  Must be 128
 *
 * @retval 0                                   If successful
 * @retval MBEDTLS_ERR_AES_INVALID_KEY_LENGTH  If keybits was not 128
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits);

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     MBEDTLS_AES_ENCRYPT or MBEDTLS_AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * @return The return value of @ref CRYPTOAesEcb.
 * @retval 0                        If successful
 * @retval AES_KEYSTORE_READ_ERROR  If the indicated keystore ram could not be read
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16],
                          unsigned char output[16]);

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/**
 * \brief          AES-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (16 bytes)
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     MBEDTLS_AES_ENCRYPT or MBEDTLS_AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_ALT_H */
