#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <yaml.h>

#include "nvm2.h"

typedef enum {
    NVM2ReceiptKeyCode_unknown,
    NVM2ReceiptKeyCode_sourceurl,
    NVM2ReceiptKeyCode_binaryurl,
    NVM2ReceiptKeyCode_sha256sum,
    NVM2ReceiptKeyCode_timestamp,
    NVM2ReceiptKeyCode_signature,
} NVM2ReceiptKeyCode;

void nvm2_receipt_dump(NVM2Receipt * receipt) {
    if (receipt == NULL) {
        return;
    }

    printf("sourceurl: %s\n", receipt->sourceurl);
    printf("binaryurl: %s\n", receipt->binaryurl);
    printf("sha256sum: %s\n", receipt->sha256sum);
    printf("signature: %s\n", receipt->signature);
    printf("timestamp: %s\n", receipt->timestamp);
}

void nvm2_receipt_free(NVM2Receipt * receipt) {
    if (receipt == NULL) {
        return;
    }

    if (receipt->sourceurl != NULL) {
        free(receipt->sourceurl);
        receipt->sourceurl = NULL;
    }

    if (receipt->binaryurl != NULL) {
        free(receipt->binaryurl);
        receipt->binaryurl = NULL;
    }

    if (receipt->sha256sum != NULL) {
        free(receipt->sha256sum);
        receipt->sha256sum = NULL;
    }

    if (receipt->timestamp != NULL) {
        free(receipt->timestamp);
        receipt->timestamp = NULL;
    }

    if (receipt->signature != NULL) {
        free(receipt->signature);
        receipt->signature = NULL;
    }

    free(receipt);
}

static NVM2ReceiptKeyCode nvm2_receipt_key_code_from_key_name(char * key) {
           if (strcmp(key, "sourceurl") == 0) {
        return NVM2ReceiptKeyCode_sourceurl;
    } else if (strcmp(key, "binaryurl") == 0) {
        return NVM2ReceiptKeyCode_binaryurl;
    } else if (strcmp(key, "sha256sum") == 0) {
        return NVM2ReceiptKeyCode_sha256sum;
    } else if (strcmp(key, "timestamp") == 0) {
        return NVM2ReceiptKeyCode_timestamp;
    } else if (strcmp(key, "signature") == 0) {
        return NVM2ReceiptKeyCode_signature;
    } else {
        return NVM2ReceiptKeyCode_unknown;
    }
}

static int nvm2_receipt_set_value(NVM2ReceiptKeyCode keyCode, char * value, NVM2Receipt * receipt) {
    if (keyCode == NVM2ReceiptKeyCode_unknown) {
        return NVM2_OK;
    }

    char c;

    for (;;) {
        c = value[0];

        if (c == '\0') {
            return NVM2_OK;
        }

        // non-printable ASCII characters and space
        if (c <= 32) {
            value = &value[1];
        } else {
            break;
        }
    }

    switch (keyCode) {
        case NVM2ReceiptKeyCode_sourceurl:
            if (receipt->sourceurl != NULL) {
                free(receipt->sourceurl);
            }

            receipt->sourceurl = strdup(value);

            if (receipt->sourceurl == NULL) {
                return NVM2_ERROR_MEMORY_ALLOCATE;
            } else {
                return NVM2_OK;
            }
        case NVM2ReceiptKeyCode_binaryurl:
            if (receipt->binaryurl != NULL) {
                free(receipt->binaryurl);
            }

            receipt->binaryurl = strdup(value);

            if (receipt->binaryurl == NULL) {
                return NVM2_ERROR_MEMORY_ALLOCATE;
            } else {
                return NVM2_OK;
            }
        case NVM2ReceiptKeyCode_sha256sum:
            if (receipt->sha256sum != NULL) {
                free(receipt->sha256sum);
            }

            receipt->sha256sum = strdup(value);

            if (receipt->sha256sum == NULL) {
                return NVM2_ERROR_MEMORY_ALLOCATE;
            } else {
                return NVM2_OK;
            }
        case NVM2ReceiptKeyCode_timestamp:
            if (receipt->timestamp != NULL) {
                free(receipt->timestamp);
            }

            receipt->timestamp = strdup(value);

            if (receipt->timestamp == NULL) {
                return NVM2_ERROR_MEMORY_ALLOCATE;
            } else {
                return NVM2_OK;
            }
        case NVM2ReceiptKeyCode_signature:
            if (receipt->signature != NULL) {
                free(receipt->signature);
            }

            receipt->signature = strdup(value);

            if (receipt->signature == NULL) {
                return NVM2_ERROR_MEMORY_ALLOCATE;
            } else {
                return NVM2_OK;
            }
        default: return NVM2_OK;
    }
}

static int nvm2_receipt_check(NVM2Receipt * receipt, const char * receiptFilePath) {
    if ((receipt->sourceurl == NULL) && (receipt->binaryurl == NULL)) {
        fprintf(stderr, "scheme error in receipt file: %s : neither sourceurl not binaryurl mapping not found.\n", receiptFilePath);
        return NVM2_ERROR_RECEIPT_SCHEME;
    }

    if (strlen(receipt->sha256sum) != 64) {
        fprintf(stderr, "scheme error in receipt file: %s : sha256sum mapping's value's length must be 64.\n", receiptFilePath);
        return NVM2_ERROR_RECEIPT_SCHEME;
    }

    if (receipt->signature == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : signature mapping not found.\n", receiptFilePath);
        return NVM2_ERROR_RECEIPT_SCHEME;
    }

    if (receipt->timestamp == NULL) {
        fprintf(stderr, "scheme error in receipt file: %s : timestamp mapping not found.\n", receiptFilePath);
        return NVM2_ERROR_RECEIPT_SCHEME;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t i = 0;
    char   c;

    for (;; i++) {
        c = receipt->timestamp[i];

        if (c == '\0') {
            break;
        }

        if ((c < '0') || (c > '9')) {
            fprintf(stderr, "scheme error in receipt file: %s : timestamp mapping's value should only contains non-numeric characters.\n", receiptFilePath);
            return NVM2_ERROR_RECEIPT_SCHEME;
        }
    }

    if (i != 10) {
        fprintf(stderr, "scheme error in receipt file: %s : timestamp mapping's value's length must be 10.\n", receiptFilePath);
        return NVM2_ERROR_RECEIPT_SCHEME;
    }

    return NVM2_OK;
}


int nvm2_receipt_parse(const char * versionName, NVM2Receipt * * out) {
    int ret = nvm2_check_if_the_given_argument_matches_version_name_pattern(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t   versionInstalledDirLength = userHomeDirLength + strlen(versionName) + 17U;
    char     versionInstalledDir[versionInstalledDirLength];
    snprintf(versionInstalledDir, versionInstalledDirLength, "%s/.nvm2/versions/%s", userHomeDir, versionName);

    struct stat st;

    if (stat(versionInstalledDir, &st) != 0) {
        return NVM2_ERROR_VERSION_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = versionInstalledDirLength + 13U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", versionInstalledDir);

    if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
        return NVM2_ERROR_VERSION_IS_BROKEN;
    }

    FILE * file = fopen(receiptFilePath, "r");

    if (file == NULL) {
        perror(receiptFilePath);
        return NVM2_ERROR;
    }

    yaml_parser_t parser;
    yaml_token_t  token;

    // https://libyaml.docsforge.com/master/api/yaml_parser_initialize/
    if (yaml_parser_initialize(&parser) == 0) {
        perror("Failed to initialize yaml parser");
        fclose(file);
        return NVM2_ERROR;
    }

    yaml_parser_set_input_file(&parser, file);

    NVM2ReceiptKeyCode receiptKeyCode = NVM2ReceiptKeyCode_unknown;

    NVM2Receipt * receipt = NULL;

    int lastTokenType = 0;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error in receipt file: %s\n", receiptFilePath);
            ret = NVM2_ERROR_RECEIPT_SYNTAX;
            goto finalize;
        }

        switch(token.type) {
            case YAML_KEY_TOKEN:
                lastTokenType = 1;
                break;
            case YAML_VALUE_TOKEN:
                lastTokenType = 2;
                break;
            case YAML_SCALAR_TOKEN:
                if (lastTokenType == 1) {
                    receiptKeyCode = nvm2_receipt_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (receipt == NULL) {
                        receipt = (NVM2Receipt*)calloc(1, sizeof(NVM2Receipt));

                        if (receipt == NULL) {
                            ret = NVM2_ERROR_MEMORY_ALLOCATE;
                            goto finalize;
                        }
                    }

                    ret = nvm2_receipt_set_value(receiptKeyCode, (char*)token.data.scalar.value, receipt);

                    if (ret != NVM2_OK) {
                        goto finalize;
                    }
                }
                break;
            default: 
                lastTokenType = 0;
                break;
        }

        if (token.type != YAML_STREAM_END_TOKEN) {
            yaml_token_delete(&token);
        }
    } while(token.type != YAML_STREAM_END_TOKEN);

finalize:
    yaml_token_delete(&token);

    yaml_parser_delete(&parser);

    fclose(file);

    if (ret == NVM2_OK) {
        ret = nvm2_receipt_check(receipt, receiptFilePath);
    }

    if (ret == NVM2_OK) {
        (*out) = receipt;
        return NVM2_OK;
    } else {
        nvm2_receipt_free(receipt);
        return ret;
    }
}
