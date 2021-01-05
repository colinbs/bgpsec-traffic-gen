#include <arpa/inet.h>
#include <stdio.h>

#include "generators.h"
#include "pdus.h"

char *generate_bytes(int amount, int mode)
{
    char *bytes = malloc(amount);

    if (!bytes)
        return NULL;

    for (int i = 0; i < amount; i++) {
        if (mode == MODE_DEC)
            bytes[i] = bytes[i] + i;
        else if (mode == MODE_HEX)
            bytes[i] = (bytes[i] + i) % 16;
    }

    return bytes;
}

struct rtr_bgpsec *generate_bgpsec_data(char asns[][11],
                                        int len,
                                        uint32_t nlri) {
    struct rtr_secure_path_seg *new_path = NULL;
    struct rtr_bgpsec *data = NULL;
    struct rtr_bgpsec_nlri pfx;
    pfx.prefix_len          = 24;
    pfx.prefix.ver          = LRTR_IPV4;
    pfx.prefix.u.addr4.addr = ntohl(nlri);
    uint32_t origin_as = *asns[len-1];

    data = rtr_mgr_bgpsec_new(1, 1, 1, origin_as, 5555, pfx);
    if (!data)
        return NULL;

    for (int i = 0; i < len; i++) {
        new_path = rtr_mgr_bgpsec_new_secure_path_seg(0, 0, atoi(asns[i]));
        if (!new_path)
            return NULL;
        rtr_mgr_bgpsec_append_sec_path_seg(data, new_path);
    }

    return data;
}

uint8_t *generate_bgpsec_attr(struct rtr_secure_path_seg *sec_path,
                              uint8_t *nlri) {
    return NULL;
}

struct rtr_signature_seg *generate_signature(
                            struct rtr_bgpsec *data,
                            struct rtr_secure_path_seg *secpath,
                            struct private_key *priv_key,
                            uint32_t nlri) {
    struct rtr_signature_seg *new_sig = NULL;
    rtr_mgr_bgpsec_generate_signature(data, priv_key->key, &new_sig);

    return new_sig;
}

struct bgpsec_upd *generate_bgpsec_upd(struct rtr_secure_path_seg *sec_path,
                                       uint8_t *nlri) {
    struct bgpsec_upd *new_upd = malloc(sizeof(struct bgpsec_upd));
    uint8_t *upd = malloc(BGPSEC_UPD_SIZE);

    if (!new_upd)
        return NULL;

    if (!upd) {
        free(new_upd);
        return NULL;
    }

    memcpy(upd, bgpsec_upd_header, BGPSEC_UPD_SIZE);

    new_upd->upd = upd;
    return new_upd;
}
