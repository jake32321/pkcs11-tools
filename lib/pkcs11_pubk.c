/* -*- mode: c; c-file-style:"stroustrup"; -*- */

/*
 * Copyright (c) 2018 Mastercard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* pkcs11_pubk.c: module to implement p11importpubk */
/* import of a public key onto a PKCS#11 token      */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/dh.h>
#include <openssl/dsa.h>
#include <openssl/ec.h>

#include "pkcs11lib.h"

/* comparison function for attributes */
static int compare_CKA( const void *a, const void *b)
{
    return ((CK_ATTRIBUTE_PTR)a)->type == ((CK_ATTRIBUTE_PTR)b)->type ? 0 : -1;
}


static EVP_PKEY * new_pubk_from_file(char *filename)
{

    EVP_PKEY * rv = NULL;

    FILE *fp = NULL;

    fp = fopen(filename,"rb"); /* open in binary mode */

    if(fp) {
	EVP_PKEY *pubk;

	/* try DER first */
	pubk = d2i_PUBKEY_fp(fp, NULL);
	fclose(fp);

	if(pubk) {
	    puts("DER format detected");
	    rv = pubk;
	} else {
	    fp = fopen(filename,"r"); /* reopen in text mode */

	    if(fp) {
		pubk = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
		fclose(fp);

		if(pubk) {
		    puts("PEM format detected");
		    rv = pubk;
		}
	    } else {
		perror("Error opening file");
	    }
	}
    } else {
	perror("Error opening file");
    }

    return rv;
}

/* RSA */

static CK_ULONG get_RSA_modulus(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_RSA  ) {
	RSA * rsa = EVP_PKEY_get1_RSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(rsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(rsa->n) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(rsa->n, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

static CK_ULONG get_RSA_public_exponent(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_RSA  ) {
	RSA * rsa = EVP_PKEY_get1_RSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(rsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(rsa->e) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(rsa->e, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}




/* DH */

static CK_ULONG get_DH_prime(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DH  ) {
	DH * dh = EVP_PKEY_get1_DH(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dh==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dh->p) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dh->p, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}


static CK_ULONG get_DH_base(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DH  ) {
	DH * dh = EVP_PKEY_get1_DH(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dh==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dh->g) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dh->g, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

static CK_ULONG get_DH_pubkey(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DH  ) {
	DH * dh = EVP_PKEY_get1_DH(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dh==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dh->pub_key) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dh->pub_key, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

/* DSA */

static CK_ULONG get_DSA_prime(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DSA  ) {
	DSA * dsa = EVP_PKEY_get1_DSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dsa->p) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dsa->p, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

static CK_ULONG get_DSA_subprime(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DSA  ) {
	DSA * dsa = EVP_PKEY_get1_DSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dsa->q) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dsa->q, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

static CK_ULONG get_DSA_base(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DSA  ) {
	DSA * dsa = EVP_PKEY_get1_DSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dsa->g) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dsa->g, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}

static CK_ULONG get_DSA_pubkey(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_DSA  ) {
	DSA * dsa = EVP_PKEY_get1_DSA(pubkey);
	CK_BYTE_PTR p = NULL;

	if(dsa==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf = OPENSSL_malloc( BN_num_bytes(dsa->pub_key) );

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = BN_bn2bin(dsa->pub_key, p);

	/* if we fail here, we would free up requested memory */
	if(rv==0) {
	    OPENSSL_free(*buf);
	    P_ERR();
	    goto error;
	}
    }
error:
    return rv;
}


/* EC */

static CK_ULONG get_EC_point(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_ULONG rv=0;
    EC_KEY* ec=NULL;
    int i2dlen=0;
    unsigned char *octp = NULL, *octbuf = NULL;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_EC  ) {

	ec = EVP_PKEY_get1_EC_KEY(pubkey);

	if(ec==NULL) {
	    P_ERR();
	    goto error;
	}

	const EC_POINT *ec_point = EC_KEY_get0_public_key(ec);
	/* get0 means no ref counter is incremented */

	if(ec_point==NULL) {
	    P_ERR();
	    goto error;
	}

	const EC_GROUP *ec_group = EC_KEY_get0_group(ec);
	/* get0 means no ref counter is incremented */

	if(ec_group==NULL) {
	    P_ERR();
	    goto error;
	}

	size_t octbuflen = EC_POINT_point2oct(ec_group, ec_point,
					      POINT_CONVERSION_UNCOMPRESSED,
					      NULL, 0, NULL);

	if(octbuflen==0) {
	    P_ERR();
	    goto error;
	}

	octp = octbuf = OPENSSL_malloc( octbuflen );

	if(octbuf==NULL) {
	    P_ERR();
	    goto error;
	}

	rv = (CK_ULONG) EC_POINT_point2oct(ec_group, ec_point,
					   POINT_CONVERSION_UNCOMPRESSED,
					   octp, octbuflen, NULL);

	if(rv==0) {
	    P_ERR();
	    OPENSSL_free(buf);
	    goto error;
	}

	/* DER-encoded of point in octbuf  */
	/* now wrap this into OCTET_STRING */

	ASN1_OCTET_STRING *wrapped = ASN1_OCTET_STRING_new();

	if(wrapped==NULL) {
	    P_ERR();
	    goto error;
	}

	if( ASN1_STRING_set(wrapped, octbuf, octbuflen) == 0 ) {
	    P_ERR();
	    goto error;
	}

	/* wrapped contains the data we need to set into buf */

	i2dlen = i2d_ASN1_OCTET_STRING(wrapped, NULL);

	if(i2dlen<0) {
	    P_ERR();
	    goto error;
	}

	CK_BYTE_PTR p = NULL;

	*buf = OPENSSL_malloc(i2dlen);

	if(*buf==NULL) {
	    P_ERR();
	    goto error;
	}

	p = *buf;

	i2dlen = i2d_ASN1_OCTET_STRING(wrapped, &p);

	if(i2dlen<0) {
	    P_ERR();
	    goto error;
	}

	rv = i2dlen;
    }
error:
    if(ec != NULL) { EC_KEY_free(ec); }
    if(octbuf != NULL) { OPENSSL_free(octbuf); }

    return rv;
}



static CK_ULONG get_EC_params(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{
    CK_LONG rv=0;
    EC_KEY* ec=NULL;

    if ( pubkey && EVP_PKEY_type(pubkey->type)==EVP_PKEY_EC  ) {

	ec = EVP_PKEY_get1_EC_KEY(pubkey);
	CK_BYTE_PTR p = NULL;

	if(ec==NULL) {
	    P_ERR();
	    goto error;
	}

	const EC_GROUP *ec_group = EC_KEY_get0_group(ec);
	/* get0 means no ref counter is incremented */

	if(ec_group==NULL) {
	    P_ERR();
	    goto error;
	}

	*buf = NULL;		/* clearing it */

	rv = i2d_ECPKParameters(ec_group, buf);

	if(rv<0) {
	    P_ERR();
	    goto error;
	}

    }
error:
    if(ec != NULL) { EC_KEY_free(ec); }

    return rv<0 ? 0 : rv ;
}


/*  get_EVP_PKEY_sha1: will retrieve RSA public key and compute SHA-1 digest
    on modulus, represented as big-endian binary digit,
    with no leading 0x00.
    This is what IBM JCE provider for PKCS#11 uses for setting CKA_ID.

    for DSA and DH, SHA-1 digest of CKA_PUBKEY is used instead.
    for EC, SHA-1 digest of CKA_EC_POINT is used instead (uncompressed form).

*/


static CK_ULONG get_EVP_PKEY_sha1(EVP_PKEY *pubkey, CK_BYTE_PTR *buf)
{

    CK_ULONG rv=0;
    if(pubkey) {
	switch(EVP_PKEY_type(pubkey->type)) {

	case EVP_PKEY_RSA:
	{
	    RSA *rsa;

	    rsa = EVP_PKEY_get1_RSA(pubkey);
	    if(rsa) {
		CK_BYTE_PTR bn_buf = OPENSSL_malloc(BN_num_bytes(rsa->n)); /* we allocate before converting */
		if(bn_buf) {
		    int bn_buf_len = BN_bn2bin(rsa->n, bn_buf);
		    {
			/* SHA-1 block */
			EVP_MD_CTX *mdctx;
			const EVP_MD *md;
			unsigned int md_len, i;

			*buf = OPENSSL_malloc(SHA_DIGEST_LENGTH); /* we allocate the buffer, and return it. */

			if(*buf) {
			    md = EVP_sha1();
			    mdctx = EVP_MD_CTX_create();
			    EVP_DigestInit_ex(mdctx, md, NULL);
			    EVP_DigestUpdate(mdctx, bn_buf, bn_buf_len);
			    EVP_DigestFinal_ex(mdctx, *buf, &md_len);
			    EVP_MD_CTX_destroy(mdctx);
			    rv = md_len;
			}
		    }
		    OPENSSL_free(bn_buf);
		}
	    }
	}
	break;


	case EVP_PKEY_DSA:
	{
	    DSA *dsa;

	    dsa = EVP_PKEY_get1_DSA(pubkey);
	    if(dsa) {
		CK_BYTE_PTR bn_buf = OPENSSL_malloc(BN_num_bytes(dsa->pub_key)); /* we allocate before converting */
		if(bn_buf) {
		    int bn_buf_len = BN_bn2bin(dsa->pub_key, bn_buf);
		    {
			/* SHA-1 block */
			EVP_MD_CTX *mdctx;
			const EVP_MD *md;
			unsigned int md_len, i;

			*buf = OPENSSL_malloc(SHA_DIGEST_LENGTH); /* we allocate the buffer, and return it. */

			if(*buf) {
			    md = EVP_sha1();
			    mdctx = EVP_MD_CTX_create();
			    EVP_DigestInit_ex(mdctx, md, NULL);
			    EVP_DigestUpdate(mdctx, bn_buf, bn_buf_len);
			    EVP_DigestFinal_ex(mdctx, *buf, &md_len);
			    EVP_MD_CTX_destroy(mdctx);
			    rv = md_len;
			}
		    }
		    OPENSSL_free(bn_buf);
		}
	    }
	}
	break;

	/* for EC, we need to retrieve the points (uncompressed), then encapsulate into an OCTETSTRING */
	/* which corresponds to the encoding on PKCS#11 */
	case EVP_PKEY_EC:
	{
	    EC_KEY *ec;

	    ec = EVP_PKEY_get1_EC_KEY(pubkey);
	    if(ec==NULL) {
		P_ERR();
	    } else {
		const EC_POINT *ec_point = EC_KEY_get0_public_key(ec);
		const EC_GROUP *ec_group = EC_KEY_get0_group(ec);


		if(ec_point==NULL) {
		    P_ERR();
		}else if (ec_group==NULL) {
		    P_ERR();
		} else {

		    /* first call to assess length of target buffer */
		    size_t ec_buflen = EC_POINT_point2oct(ec_group, ec_point,
							  POINT_CONVERSION_UNCOMPRESSED,
							  NULL, 0, NULL);

		    if(ec_buflen==0) {
			P_ERR();
		    } else {

			unsigned char *p, *ec_buf;

			p = ec_buf = OPENSSL_malloc( ec_buflen );

			if(ec_buf==NULL) {
			    P_ERR();
			} else {
			    /* second call to obtain DER-encoded point */
			    rv = (CK_ULONG) EC_POINT_point2oct(ec_group, ec_point,
							       POINT_CONVERSION_UNCOMPRESSED,
							       p, ec_buflen, NULL);
			    if(rv==0) {
				P_ERR();
			    } else {

				/* now start the wrapping to OCTET STRING business */

				ASN1_OCTET_STRING *wrapped = ASN1_OCTET_STRING_new();

				if(wrapped==NULL) {
				    P_ERR();
				} else {
				    if( ASN1_STRING_set(wrapped, ec_buf, ec_buflen) == 0 ) {
					P_ERR();
				    } else {
					/* wrapped contains the data we need to set into buf */

					/* determine length of buffer */
					int i2dlen = i2d_ASN1_OCTET_STRING(wrapped, NULL);

					if(i2dlen<0) {
					    P_ERR();
					} else {

					    CK_BYTE_PTR p = NULL, wrapbuf = NULL;

					    wrapbuf = OPENSSL_malloc(i2dlen);

					    if(wrapbuf==NULL) {
						P_ERR();
					    } else {

						p = wrapbuf;

						i2dlen = i2d_ASN1_OCTET_STRING(wrapped, &p);

						if(i2dlen<0) {
						    P_ERR();
						} else {

						    /* SHA-1 block */
						    EVP_MD_CTX *mdctx;
						    const EVP_MD *md;
						    unsigned int md_len, i;

						    *buf = OPENSSL_malloc(SHA_DIGEST_LENGTH); /* we allocate the buffer, and return it. */

						    if(*buf ==NULL) {
							P_ERR();
						    } else {
							md = EVP_sha1();
							mdctx = EVP_MD_CTX_create();
							EVP_DigestInit_ex(mdctx, md, NULL);
							EVP_DigestUpdate(mdctx, wrapbuf, i2dlen);
							EVP_DigestFinal_ex(mdctx, *buf, &md_len);
							EVP_MD_CTX_destroy(mdctx);
							rv = md_len;
						    }
						}
					    }
					    OPENSSL_free(wrapbuf);
					}
				    }
				    ASN1_OCTET_STRING_free(wrapped);
				}

			    }
			    OPENSSL_free(ec_buf);
			}
		    }
		    /* get0 on ec_point & ec_group, we can safely forget */
		}
		EC_KEY_free(ec);
	    }
	}
	break;


	case EVP_PKEY_DH:
	{
	    DH *dh;

	    dh = EVP_PKEY_get1_DH(pubkey);
	    if(dh) {
		CK_BYTE_PTR bn_buf = OPENSSL_malloc(BN_num_bytes(dh->pub_key)); /* we allocate before converting */
		if(bn_buf) {
		    int bn_buf_len = BN_bn2bin(dh->pub_key, bn_buf);
		    {
			/* SHA-1 block */
			EVP_MD_CTX *mdctx;
			const EVP_MD *md;
			unsigned int md_len, i;

			*buf = OPENSSL_malloc(SHA_DIGEST_LENGTH); /* we allocate the buffer, and return it. */

			if(*buf) {
			    md = EVP_sha1();
			    mdctx = EVP_MD_CTX_create();
			    EVP_DigestInit_ex(mdctx, md, NULL);
			    EVP_DigestUpdate(mdctx, bn_buf, bn_buf_len);
			    EVP_DigestFinal_ex(mdctx, *buf, &md_len);
			    EVP_MD_CTX_destroy(mdctx);
			    rv = md_len;
			}
		    }
		    OPENSSL_free(bn_buf);
		}
	    }
	}
	break;

	default:
	    break;

	}
    }
    return rv;
}




CK_OBJECT_HANDLE pkcs11_importpubk( pkcs11Context * p11Context,
				    char *filename,
				    char *label,
				    int trusted,
				    CK_ATTRIBUTE attrs[],
				    CK_ULONG numattrs )
{

    CK_OBJECT_HANDLE hPubk = NULL_PTR;

    CK_RV retCode;
    CK_OBJECT_CLASS pubkClass = CKO_PUBLIC_KEY;
    CK_KEY_TYPE pubkType;

    CK_BBOOL false = CK_FALSE;
    CK_BBOOL true = CK_TRUE;

    EVP_PKEY *pubk = NULL;

    /* now let's create the attributes from file and alias */

    pubk = new_pubk_from_file(filename);

    if(pubk) {

	switch( EVP_PKEY_type(pubk->type) ) {

	case EVP_PKEY_RSA: {


	    CK_BYTE_PTR pubkey_hash = NULL;
	    CK_ULONG pubkey_hash_len = 0;

	    CK_BYTE_PTR rsa_modulus = NULL;
	    CK_ULONG rsa_modulus_len = 0;

	    CK_BYTE_PTR rsa_public_exponent = NULL;
	    CK_ULONG rsa_public_exponent_len =0;

	    CK_ATTRIBUTE pubkTemplate[] = {
		{CKA_CLASS, &pubkClass, sizeof(pubkClass)},          /* 0  */
		{CKA_KEY_TYPE, &pubkType, sizeof(pubkType)},	     /* 1  */
		{CKA_ID, NULL, 0},				     /* 2  */
		{CKA_LABEL, label, strlen(label) },		     /* 3  */
		{CKA_ENCRYPT, &true, sizeof(true) },		     /* 4  */
		{CKA_WRAP, &true, sizeof(true) },		     /* 5  */
		{CKA_VERIFY, &true, sizeof(true) },		     /* 6  */
		{CKA_VERIFY_RECOVER, &true, sizeof(true) },	     /* 7  */
		{CKA_TOKEN, &true, sizeof(true)},		     /* 8  */
		{CKA_MODULUS, NULL, 0 },                             /* 9  */
		{CKA_PUBLIC_EXPONENT, NULL, 0 },                     /* 10 */
		{CKA_MODIFIABLE, &true, sizeof(CK_BBOOL) },	     /* 11 */
		{CKA_TRUSTED, &true, sizeof(CK_BBOOL) },	     /* 12 */
		/* CKA_TRUSTED set at last position   */
		/* this flag is FALSE by default      */
		/* So we don't present it in case     */
		/* library does not support attribute */
		/* if trust flag is needed, then we expand */
		/* the size of the structure by 1     */
	    };

	    pubkType = CKK_RSA;

	    pubkey_hash_len = get_EVP_PKEY_sha1( pubk, &pubkey_hash);
	    rsa_modulus_len = get_RSA_modulus( pubk, &rsa_modulus);
	    rsa_public_exponent_len = get_RSA_public_exponent( pubk, &rsa_public_exponent);

	    if( pubkey_hash_len >0 && rsa_modulus_len>0 && rsa_public_exponent_len>0) {

		/* we have everything, let's fill in the template */

		pubkTemplate[2].pValue = pubkey_hash; /* CKA_ID */
		pubkTemplate[2].ulValueLen = pubkey_hash_len;

		pubkTemplate[9].pValue = rsa_modulus;
		pubkTemplate[9].ulValueLen = rsa_modulus_len;

		pubkTemplate[10].pValue = rsa_public_exponent;
		pubkTemplate[10].ulValueLen = rsa_public_exponent_len;

		/* let's override with any boolean attribute given in the command line */
		/* we consider only the following attributes: */
		/* CKA_ENCRYPT                                */
		/* CKA_WRAP                                   */
		/* CKA_VERIFY                                 */
		/* CKA_VERIFY_RECOVER                         */
		/* CKA_MODIFIABLE                             */
		/* by default, all these attributes are set to true */
		/* note that CKA_TRUSTED is handled separately  */
		int i;

		for(i=0; i<numattrs; i++) {
		    /* lsearch will add the keys if not found in the template */

		    size_t arglen = sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE);

		    CK_ATTRIBUTE_PTR match = (CK_ATTRIBUTE_PTR) lfind( &attrs[i],
								       pubkTemplate,
								       &arglen,
								       sizeof(CK_ATTRIBUTE),
								       compare_CKA );

		    /* do we have a match in the template list? */
		    if(match) {
			switch(match->type) {
			case CKA_ENCRYPT:
			case CKA_WRAP:
			case CKA_VERIFY:
			case CKA_VERIFY_RECOVER:
			case CKA_MODIFIABLE:
			    match->pValue = attrs[i].pValue; /* we use the value passed as argument. Do not free it afterwards! */
			    break;

			default:
			    break;
			    /* we do nothing */
			}
		    }
		}

		/* if -T is set: we want trusted */
		if(trusted) {
		    pubkTemplate[ (sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE))-2 ].pValue = &false; /* then CKA_MODIFIABLE must be false */
		}

		retCode = p11Context->FunctionList.C_CreateObject(p11Context->Session,
								  pubkTemplate,
								  (trusted ? sizeof(pubkTemplate) : sizeof(pubkTemplate)-2) / sizeof(CK_ATTRIBUTE),
								  &hPubk);

		pkcs11_error( retCode, "CreateObject" );

		/* if we are here, we have to free up memory anyway */
	    }

	    if(pubkey_hash) { OPENSSL_free(pubkey_hash); }
	    if(rsa_modulus) { OPENSSL_free(rsa_modulus); }
	    if(rsa_public_exponent) { OPENSSL_free(rsa_public_exponent); }

	}
	    break;

	case EVP_PKEY_DSA: {

	    CK_BYTE_PTR pubkey_hash = NULL;
	    CK_ULONG pubkey_hash_len = 0;

	    CK_BYTE_PTR dsa_prime = NULL;
	    CK_ULONG dsa_prime_len = 0;

	    CK_BYTE_PTR dsa_subprime = NULL;
	    CK_ULONG dsa_subprime_len = 0;

	    CK_BYTE_PTR dsa_base = NULL;
	    CK_ULONG dsa_base_len = 0;

	    CK_BYTE_PTR dsa_pubkey = NULL;
	    CK_ULONG dsa_pubkey_len = 0;


	    CK_ATTRIBUTE pubkTemplate[] = {
		{CKA_CLASS, &pubkClass, sizeof(pubkClass)},          /* 0  */
		{CKA_KEY_TYPE, &pubkType, sizeof(pubkType)},	     /* 1  */
		{CKA_ID, NULL, 0},				     /* 2  */
		{CKA_LABEL, label, strlen(label) },		     /* 3  */
		{CKA_VERIFY, &true, sizeof(true) },		     /* 4  */
		{CKA_TOKEN, &true, sizeof(true)},		     /* 5  */
		{CKA_PRIME, NULL, 0 },                               /* 6  */
		{CKA_SUBPRIME, NULL, 0 },                            /* 7  */
		{CKA_BASE, NULL, 0 },                                /* 8  */
		{CKA_VALUE, NULL, 0 },                               /* 9  */
		{CKA_MODIFIABLE, &true, sizeof(CK_BBOOL) },	     /* 10 */
		{CKA_TRUSTED, &true, sizeof(CK_BBOOL) },	     /* 11 */
		/* CKA_TRUSTED set at last position   */
		/* this flag is FALSE by default      */
		/* So we don't present it in case     */
		/* library does not support attribute */
		/* if trust flag is needed, then we expand */
		/* the size of the structure by 1     */
	    };

	    pubkType = CKK_DSA;


	    pubkey_hash_len = get_EVP_PKEY_sha1( pubk, &pubkey_hash);

	    dsa_prime_len = get_DSA_prime( pubk, &dsa_prime);          /* p */
	    dsa_subprime_len = get_DSA_subprime( pubk, &dsa_subprime); /* q */
	    dsa_base_len = get_DSA_base( pubk, &dsa_base);             /* g */
	    dsa_pubkey_len = get_DSA_pubkey( pubk, &dsa_pubkey);       /* public key */

	    if( pubkey_hash_len >0 &&
		dsa_prime_len > 0 &&
		dsa_subprime_len > 0 &&
		dsa_base_len > 0 &&
		dsa_pubkey_len > 0 ) {

		/* we have everything, let's fill in the template */

		pubkTemplate[2].pValue = pubkey_hash; /* CKA_ID */
		pubkTemplate[2].ulValueLen = pubkey_hash_len;

		pubkTemplate[6].pValue = dsa_prime;
		pubkTemplate[6].ulValueLen = dsa_prime_len;

		pubkTemplate[7].pValue = dsa_subprime;
		pubkTemplate[7].ulValueLen = dsa_subprime_len;

		pubkTemplate[8].pValue = dsa_base;
		pubkTemplate[8].ulValueLen = dsa_base_len;

		pubkTemplate[9].pValue = dsa_pubkey;
		pubkTemplate[9].ulValueLen = dsa_pubkey_len;

		/* let's override with any boolean attribute given in the command line */
		/* we consider only the following attributes: */
		/* CKA_VERIFY                                 */
		/* CKA_MODIFIABLE                             */
		/* by default, all these attributes are set to true */
		/* note that CKA_TRUSTED is handled separately  */
		int i;

		for(i=0; i<numattrs; i++) {
		    /* lsearch will add the keys if not found in the template */

		    size_t arglen = sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE);

		    CK_ATTRIBUTE_PTR match = (CK_ATTRIBUTE_PTR) lfind( &attrs[i],
								       pubkTemplate,
								       &arglen,
								       sizeof(CK_ATTRIBUTE),
								       compare_CKA );

		    /* do we have a match in the template list? */
		    if(match) {
			switch(match->type) {
			case CKA_VERIFY:
			case CKA_MODIFIABLE:
			    match->pValue = attrs[i].pValue; /* we use the value passed as argument. Do not free it afterwards! */
			    break;

			default:
			    break;
			    /* we do nothing */
			}
		    }
		}


		/* if -T is set: we want trusted */
		if(trusted) {
		    pubkTemplate[ (sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE))-2 ].pValue = &false; /* then CKA_MODIFIABLE must be false */
		}

		retCode = p11Context->FunctionList.C_CreateObject(p11Context->Session,
								  pubkTemplate,
								  (trusted ? sizeof(pubkTemplate) : sizeof(pubkTemplate)-2) / sizeof(CK_ATTRIBUTE),
								  &hPubk);

		pkcs11_error( retCode, "CreateObject" );

		/* if we are here, we have to free up memory anyway */
	    }

	    if(pubkey_hash)  { OPENSSL_free(pubkey_hash); }
	    if(dsa_prime)    { OPENSSL_free(dsa_prime); }
	    if(dsa_subprime) { OPENSSL_free(dsa_subprime); }
	    if(dsa_base)     { OPENSSL_free(dsa_base); }
	    if(dsa_pubkey)   { OPENSSL_free(dsa_pubkey); }

	}
	    break;

	case EVP_PKEY_DH: {

	    CK_BYTE_PTR pubkey_hash = NULL;
	    CK_ULONG pubkey_hash_len = 0;

	    CK_BYTE_PTR dh_prime = NULL;
	    CK_ULONG dh_prime_len = 0;

	    CK_BYTE_PTR dh_base = NULL;
	    CK_ULONG dh_base_len = 0;

	    CK_BYTE_PTR dh_pubkey = NULL;
	    CK_ULONG dh_pubkey_len = 0;


	    CK_ATTRIBUTE pubkTemplate[] = {
		{CKA_CLASS, &pubkClass, sizeof(pubkClass)},          /* 0  */
		{CKA_KEY_TYPE, &pubkType, sizeof(pubkType)},	     /* 1  */
		{CKA_ID, NULL, 0},				     /* 2  */
		{CKA_LABEL, label, strlen(label) },		     /* 3  */
		{CKA_DERIVE, &true, sizeof(true) },		     /* 4  */
		{CKA_TOKEN, &true, sizeof(true)},		     /* 5  */
		{CKA_PRIME, NULL, 0 },                               /* 6  */
		{CKA_BASE, NULL, 0 },                                /* 7  */
		{CKA_VALUE, NULL, 0 },                               /* 8  */
		{CKA_MODIFIABLE, &true, sizeof(CK_BBOOL) },	     /* 9  */
		{CKA_TRUSTED, &true, sizeof(CK_BBOOL) },	     /* 10 */
		/* CKA_TRUSTED set at last position   */
		/* this flag is FALSE by default      */
		/* So we don't present it in case     */
		/* library does not support attribute */
		/* if trust flag is needed, then we expand */
		/* the size of the structure by 1     */
	    };

	    pubkType = CKK_DH;


	    pubkey_hash_len = get_EVP_PKEY_sha1( pubk, &pubkey_hash);

	    dh_prime_len = get_DH_prime( pubk, &dh_prime);          /* p */
	    dh_base_len = get_DH_base( pubk, &dh_base);              /* g */
	    dh_pubkey_len = get_DH_pubkey( pubk, &dh_pubkey);        /* public key */

	    if( pubkey_hash_len >0 &&
		dh_prime_len > 0 &&
		dh_base_len > 0 &&
		dh_pubkey_len > 0 ) {

		/* we have everything, let's fill in the template */

		pubkTemplate[2].pValue = pubkey_hash; /* CKA_ID */
		pubkTemplate[2].ulValueLen = pubkey_hash_len;

		pubkTemplate[6].pValue = dh_prime;
		pubkTemplate[6].ulValueLen = dh_prime_len;

		pubkTemplate[7].pValue = dh_base;
		pubkTemplate[7].ulValueLen = dh_base_len;

		pubkTemplate[8].pValue = dh_pubkey;
		pubkTemplate[8].ulValueLen = dh_pubkey_len;

		/* let's override with any boolean attribute given in the command line */
		/* we consider only the following attributes: */
		/* CKA_DERIVE                                 */
		/* CKA_MODIFIABLE                             */
		/* by default, all these attributes are set to true */
		/* note that CKA_TRUSTED is handled separately  */
		int i;

		for(i=0; i<numattrs; i++) {
		    /* lsearch will add the keys if not found in the template */

		    size_t arglen = sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE);

		    CK_ATTRIBUTE_PTR match = (CK_ATTRIBUTE_PTR) lfind( &attrs[i],
								       pubkTemplate,
								       &arglen,
								       sizeof(CK_ATTRIBUTE),
								       compare_CKA );

		    /* do we have a match in the template list? */
		    if(match) {
			switch(match->type) {
			case CKA_DERIVE:
			case CKA_MODIFIABLE:
			    match->pValue = attrs[i].pValue; /* we use the value passed as argument. Do not free it afterwards! */
			    break;

			default:
			    break;
			    /* we do nothing */
			}
		    }
		}

		/* if -T is set: we want trusted */
		if(trusted) {
		    pubkTemplate[ (sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE))-2 ].pValue = &false; /* then CKA_MODIFIABLE must be false */
		}

		retCode = p11Context->FunctionList.C_CreateObject(p11Context->Session,
								  pubkTemplate,
								  (trusted ? sizeof(pubkTemplate) : sizeof(pubkTemplate)-2) / sizeof(CK_ATTRIBUTE),
								  &hPubk);

		pkcs11_error( retCode, "CreateObject" );

		/* if we are here, we have to free up memory anyway */
	    }

	    if(pubkey_hash)  { OPENSSL_free(pubkey_hash); }
	    if(dh_prime)    { OPENSSL_free(dh_prime); }
	    if(dh_base)     { OPENSSL_free(dh_base); }
	    if(dh_pubkey)   { OPENSSL_free(dh_pubkey); }

	}
	    break;


	case EVP_PKEY_EC: {

	    CK_BYTE_PTR pubkey_hash = NULL;
	    CK_ULONG pubkey_hash_len = 0;

	    CK_BYTE_PTR ec_params = NULL;
	    CK_ULONG ec_params_len = 0;

	    CK_BYTE_PTR ec_point = NULL;
	    CK_ULONG ec_point_len = 0;

	    CK_ATTRIBUTE pubkTemplate[] = {
		{CKA_CLASS, &pubkClass, sizeof(pubkClass)},          /* 0  */
		{CKA_KEY_TYPE, &pubkType, sizeof(pubkType)},	     /* 1  */
		{CKA_ID, NULL, 0},				     /* 2  */
		{CKA_LABEL, label, strlen(label) },		     /* 3  */
		{CKA_VERIFY, &true, sizeof(true) },		     /* 4  */
		{CKA_DERIVE, &false, sizeof(false)},                 /* 5  */
		{CKA_TOKEN, &true, sizeof(true)},		     /* 6  */
		{CKA_EC_PARAMS, NULL, 0 },                           /* 7  */
		{CKA_EC_POINT, NULL, 0 },                            /* 8  */
		{CKA_MODIFIABLE, &true, sizeof(CK_BBOOL) },	     /* 9  */
		{CKA_TRUSTED, &true, sizeof(CK_BBOOL) },	     /* 10 */
		/* CKA_TRUSTED set at last position   */
		/* this flag is FALSE by default      */
		/* So we don't present it in case     */
		/* library does not support attribute */
		/* if trust flag is needed, then we expand */
		/* the size of the structure by 1     */
	    };

	    pubkType = CKK_EC;


	    pubkey_hash_len = get_EVP_PKEY_sha1( pubk, &pubkey_hash);

	    ec_params_len = get_EC_params( pubk, &ec_params);           /* curve parameters */
	    ec_point_len  = get_EC_point( pubk, &ec_point);             /* curve point */

	    if( pubkey_hash_len >0 &&
		ec_params_len > 0 &&
		ec_point_len > 0 ) {

		/* we have everything, let's fill in the template */

		pubkTemplate[2].pValue = pubkey_hash; /* CKA_ID */
		pubkTemplate[2].ulValueLen = pubkey_hash_len;

		pubkTemplate[7].pValue = ec_params;
		pubkTemplate[7].ulValueLen = ec_params_len;

		pubkTemplate[8].pValue = ec_point;
		pubkTemplate[8].ulValueLen = ec_point_len;

		/* let's override with any boolean attribute given in the command line */
		/* we consider only the following attributes: */
		/* CKA_VERIFY                                 */
		/* CKA_DERIVE                                 */
		/* CKA_MODIFIABLE                             */
		/* by default, all these attributes are set to true */
		/* note that CKA_TRUSTED is handled separately  */
		int i;

		for(i=0; i<numattrs; i++) {
		    /* lsearch will add the keys if not found in the template */

		    size_t arglen = sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE);

		    CK_ATTRIBUTE_PTR match = (CK_ATTRIBUTE_PTR) lfind( &attrs[i],
								       pubkTemplate,
								       &arglen,
								       sizeof(CK_ATTRIBUTE),
								       compare_CKA );

		    /* do we have a match in the template list? */
		    if(match) {
			switch(match->type) {
			case CKA_VERIFY:
			case CKA_DERIVE:
			case CKA_MODIFIABLE:
			    match->pValue = attrs[i].pValue; /* we use the value passed as argument. Do not free it afterwards! */
			    break;

			default:
			    break;
			    /* we do nothing */
			}
		    }
		}

		/* if -T is set: we want trusted */
		if(trusted) {
		    pubkTemplate[ (sizeof(pubkTemplate) / sizeof(CK_ATTRIBUTE))-2 ].pValue = &false; /* then CKA_MODIFIABLE must be false */
		}

		retCode = p11Context->FunctionList.C_CreateObject(p11Context->Session,
								  pubkTemplate,
								  (trusted ? sizeof(pubkTemplate) : sizeof(pubkTemplate)-2) / sizeof(CK_ATTRIBUTE),
								  &hPubk);

		pkcs11_error( retCode, "CreateObject" );

		/* if we are here, we have to free up memory anyway */
	    }

	    if(pubkey_hash)  { OPENSSL_free(pubkey_hash); }
	    if(ec_params)    { OPENSSL_free(ec_params); }
	    if(ec_point)     { OPENSSL_free(ec_point); }
	}
	    break;


	default:
	    fprintf(stderr, "***ERROR - public key type not supported\n");
	    break;
	}

	OPENSSL_free(pubk);

    }
    return hPubk;
}
