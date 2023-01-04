/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "FreeRTOS.h"

#include "sln_encrypt.h"
#include "fsl_dcp.h"
//#include "fsl_caam.h"

//static caam_handle_t s_caamHandle[] = {
//    [0] = {.jobRing = kCAAM_JobRing1}, [1] = {.jobRing = kCAAM_JobRing2}, [2] = {.jobRing = kCAAM_JobRing3}};

static dcp_handle_t m_handle[] = {
		[0] = {.channel = kDCP_Channel0,
		.swapConfig = kDCP_NoSwap,
		.keySlot = kDCP_KeySlot0},
		[1] = {.channel = kDCP_Channel1,
		.swapConfig = kDCP_NoSwap,
		.keySlot = kDCP_KeySlot1},
		[2] = {.channel = kDCP_Channel2,
		.swapConfig = kDCP_NoSwap,
		.keySlot = kDCP_KeySlot2},
		[3] = {.channel = kDCP_Channel3,
		.swapConfig = kDCP_NoSwap,
		.keySlot = kDCP_KeySlot3},
};


static const void *s_SNL_EncryptCtx[SLN_ENCRYPT_SLOTS] = {0};
static uint16_t s_caamUsers                            = 0;

static void SLN_Encrypt_Attach_Key(const void *ctx, uint8_t keySlot)
{
    s_SNL_EncryptCtx[keySlot] = ctx;
}

static void SLN_Encrypt_Detach_Key(const void *ctx)
{
    for (int32_t i = 0; i < SLN_ENCRYPT_SLOTS; i++)
    {
        if (ctx == s_SNL_EncryptCtx[i])
        {
            s_SNL_EncryptCtx[i] = NULL;
            break;
        }
    }
}

static bool SLN_Encrypt_Key_Loaded(const void *ctx)
{
    bool ret = false;

    for (int32_t i = 0; i < SLN_ENCRYPT_SLOTS; i++)
    {
        if (ctx == s_SNL_EncryptCtx[i])
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static bool SLN_Encrypt_KeySlot_Busy(uint8_t keySlot)
{
    const sln_encrypt_ctx_t *ctx;

    for (int32_t i = 0; i < SLN_ENCRYPT_SLOTS; i++)
    {
        ctx = s_SNL_EncryptCtx[i];

        if (ctx != NULL && keySlot == ctx->keySlot)
        {
            return true;
        }
    }

    return false;
}

int32_t SLN_Encrypt_Init_Slot(sln_encrypt_ctx_t *ctx)
{
    if (ctx == NULL)
    {
        return SLN_ENCRYPT_NULL_CTX;
    }

    if (ctx->keySize != 16 && ctx->keySize != 24 && ctx->keySize != 32)
    {
        return SLN_ENCRYPT_INVALID_KEY;
    }

    if (SLN_Encrypt_KeySlot_Busy(ctx->keySlot))
    {
        return SLN_ENCRYPT_KEYSLOT_BUSY;
    }

    SLN_Encrypt_Attach_Key(ctx, ctx->keySlot);
    /*
     * Note: in case CAAM_Init() will not be called from CRYPTO_InitHardware(), we
     * can call it from here when s_caamUsers is 0.
     */
    if (s_caamUsers == 0)
    {
    	dcp_config_t dcpConfig;
		/* Initialize DCP */
		DCP_GetDefaultConfig(&dcpConfig);
	    /* Reset and initialize DCP */
	    DCP_Init(DCP, &dcpConfig);
    }

    s_caamUsers++;

    return SLN_ENCRYPT_STATUS_OK;
}

int32_t SLN_Encrypt_Deinit_Slot(sln_encrypt_ctx_t *ctx)
{
    if (s_caamUsers == 0)
    {
        return SLN_ENCRYPT_STATUS_OK;
    }

    if (ctx == NULL)
    {
        return SLN_ENCRYPT_NULL_CTX;
    }

    if (SLN_Encrypt_Key_Loaded(ctx))
    {
        SLN_Encrypt_Detach_Key(ctx);
    }

    s_caamUsers--;

    if (s_caamUsers == 0)
    {
        /* Deinitialize DCP */
        DCP_Deinit(DCP);
    }

    return SLN_ENCRYPT_STATUS_OK;
}

int32_t SLN_Encrypt_AES_CBC_PKCS7(
    sln_encrypt_ctx_t *ctx, const uint8_t *in, size_t inSize, uint8_t *out, size_t outSize)
{
    uint32_t keySize;
    uint8_t *iv, *encKey, keySlot;
    uint32_t alignedSize, lastSize;
    int32_t ret = SLN_ENCRYPT_STATUS_OK;

    iv      = (uint8_t *)ctx->iv;
    encKey  = (uint8_t *)ctx->key;
    keySlot = ctx->keySlot;
    keySize = ctx->keySize;

    if ((outSize % AES_BLOCK_SIZE) && (outSize < inSize))
    {
        ret = SLN_ENCRYPT_WRONG_OUT_BUFSIZE;
        goto exit;
    }

    if (!SLN_Encrypt_Key_Loaded(ctx))
    {
        ret = SLN_ENCRYPT_KEYSLOT_INVALID;
        goto exit;
    }

    alignedSize = (inSize / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    lastSize    = inSize % AES_BLOCK_SIZE;

    /* Encrypt the 16-byte aligned part first */
    if (alignedSize)
    {
//        ret = CAAM_AES_EncryptCbc(CAAM, &s_caamHandle[keySlot], in, out, alignedSize, iv, encKey, keySize);
    	status_t status = DCP_AES_SetKey(DCP, &m_handle[keySlot], encKey, keySize);
        status = DCP_AES_EncryptCbc(DCP, &m_handle[keySlot], in, out, alignedSize, iv);
        if (status != kStatus_Success)
        {
            ret = SLN_ENCRYPT_ENCRYPT_ERROR_1;
            goto exit;
        }
    }

    /* And the rest copy in a 16-byte buffer */
    if (lastSize)
    {
        uint8_t pad = AES_BLOCK_SIZE - lastSize;
        uint8_t alignedInBuf[AES_BLOCK_SIZE];

        memset(alignedInBuf, 0, AES_BLOCK_SIZE);
        memcpy(alignedInBuf, in + alignedSize, lastSize);

        /* Implement PKCS#7 padding so we can recover plain text length later */
        if (pad)
        {
            memset(alignedInBuf + lastSize, pad, pad);
        }

//        ret = CAAM_AES_EncryptCbc(CAAM, &s_caamHandle[keySlot], alignedInBuf, out + alignedSize, AES_BLOCK_SIZE, iv,
//                                  encKey, keySize);
    	status_t status = DCP_AES_SetKey(DCP, &m_handle[keySlot], encKey, keySize);
        status = DCP_AES_EncryptCbc(DCP, &m_handle[keySlot], alignedInBuf, out + alignedSize, AES_BLOCK_SIZE, iv);
        if (status != kStatus_Success)
        {
            ret = SLN_ENCRYPT_ENCRYPT_ERROR_1;
            goto exit;
        }
    }

exit:
    return ret;
}

int32_t SLN_Decrypt_AES_CBC_PKCS7(
    sln_encrypt_ctx_t *ctx, const uint8_t *in, size_t inSize, uint8_t *out, size_t *outSize)
{
    uint32_t keySize = 0;
    ;
    uint8_t *iv          = NULL;
    uint8_t *decKey      = NULL;
    uint8_t keySlot      = 0;
    uint32_t alignedSize = 0;
    uint32_t lastSize    = 0;
    int32_t ret          = SLN_ENCRYPT_STATUS_OK;
    uint8_t pad          = 0;

    iv      = (uint8_t *)ctx->iv;
    decKey  = (uint8_t *)ctx->key;
    keySlot = ctx->keySlot;
    keySize = ctx->keySize;

    if ((inSize % AES_BLOCK_SIZE) && (*outSize < inSize))
    {
        ret = SLN_ENCRYPT_WRONG_IN_BUFSIZE;
        goto exit;
    }

    if (!SLN_Encrypt_Key_Loaded(ctx))
    {
        ret = SLN_ENCRYPT_KEYSLOT_INVALID;
        goto exit;
    }

    alignedSize = (inSize > AES_BLOCK_SIZE) ? inSize - AES_BLOCK_SIZE : 0;
    lastSize    = AES_BLOCK_SIZE;

    /* Decrypt the 16-byte aligned part first */
    if (alignedSize)
    {
//        ret = CAAM_AES_DecryptCbc(CAAM, &s_caamHandle[keySlot], in, out, inSize, iv, decKey, keySize);
    	status_t status = DCP_AES_SetKey(DCP, &m_handle[keySlot], decKey, keySize);
        status = DCP_AES_DecryptCbc(DCP, &m_handle[keySlot], in, out, inSize, iv);
        if (status != kStatus_Success)
        {
            ret = SLN_ENCRYPT_DECRYPT_ERROR_1;
            goto exit;
        }

    }

    /* And the rest copy in a 16-byte buffer */
    uint8_t alignedOutBuf[AES_BLOCK_SIZE];

//    ret = CAAM_AES_DecryptCbc(CAAM, &s_caamHandle[keySlot], in + alignedSize, alignedOutBuf, AES_BLOCK_SIZE, iv, decKey,
//                              keySize);

	status_t status = DCP_AES_SetKey(DCP, &m_handle[keySlot], decKey, keySize);
    status = DCP_AES_DecryptCbc(DCP, &m_handle[keySlot], in + alignedSize, alignedOutBuf, AES_BLOCK_SIZE, iv);
    if (status != kStatus_Success)
    {
        ret = SLN_ENCRYPT_ENCRYPT_ERROR_1;
        goto exit;
    }

    if (SLN_ENCRYPT_STATUS_OK != ret)
    {
        ret = SLN_ENCRYPT_DECRYPT_ERROR_2;
        goto exit;
    }

    /* Work out plain length from PKCS#7 padding */
    pad = alignedOutBuf[AES_BLOCK_SIZE - 1];

    if (pad < AES_BLOCK_SIZE)
    {
        /* Verify padding makes sense */
        uint32_t startIdx = AES_BLOCK_SIZE - 1;
        uint32_t endIdx   = startIdx - pad;
        for (uint32_t idx = startIdx; idx > endIdx; idx--)
        {
            if (alignedOutBuf[idx] != pad)
            {
                // Not valid padding, assume input length is output length
                pad = 0;
                break;
            }
        }

        lastSize -= pad;
    }

    /* Copy final data into output buffer */
    memcpy(out + alignedSize, alignedOutBuf, lastSize);

    /* Update output size */
    *outSize = alignedSize + lastSize;

exit:
    return SLN_ENCRYPT_STATUS_OK;
}

int32_t SLN_Crc(sln_encrypt_ctx_t *ctx, const uint8_t *in, size_t inSize, uint32_t *out, size_t *outSize)
{
    int32_t ret     = SLN_ENCRYPT_STATUS_OK;
    status_t status = kStatus_Success;

    uint8_t keySlot = ctx->keySlot;
//    caam_hash_ctx_t ctx_caam;
    if ((NULL == ctx))
    {
        ret = SLN_ENCRYPT_NULL_CTX;
        goto exit;
    }

    if (!SLN_Encrypt_Key_Loaded(ctx))
    {
        ret = SLN_ENCRYPT_KEYSLOT_INVALID;
        goto exit;
    }

    if ((NULL == in) || (NULL == out) || (NULL == outSize))
    {
        ret = SLN_ENCRYPT_NULL_PARAM;
        goto exit;
    }

    status = DCP_HASH(DCP, &m_handle[keySlot], kDCP_Sha256, in, inSize, out, outSize);
//    status = CAAM_HASH_Init(CAAM, &s_caamHandle[keySlot], &ctx_caam, kCAAM_Sha256, NULL, 0u);

    if (kStatus_Success != status)
    {
        ret = SLN_ENCRYPT_CRC_ERROR_1;
        goto exit;
    }

#if 0
//    status = CAAM_HASH_Update(&ctx_caam, in, inSize);

    if (kStatus_Success != status)
    {
        ret = SLN_ENCRYPT_CRC_ERROR_2;
        goto exit;
    }

//    status = CAAM_HASH_Finish(&ctx_caam, out, NULL);

    if (kStatus_Success != status)
    {
        ret = SLN_ENCRYPT_CRC_ERROR_2;
        goto exit;
    }
    /* 256 bits divided by 8 */
    *outSize = 32;
#endif

exit:
    return ret;
}
