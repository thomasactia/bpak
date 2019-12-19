/**
 * BPAK - Bit Packer
 *
 * Copyright (C) 2019 Jonas Blixt <jonpe960@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef INCLUDE_BPAK_BPAK_H_
#define INCLUDE_BPAK_BPAK_H_

#include <stdint.h>

#define BPAK_HEADER_MAGIC 0xd24f859a
#define BPAK_TAG_MAGIC 0x76650c19

/*
 * Internal tags:
 *
 *  TAG                           Encoder/Decoder
 *  ---                           ---------------
 *  bpak-signature                DER
 *  bpak-key-hint                 uint32
 *  bpak-transport-signature      DER
 *  bpak-transport-signature-hint uint32
 *  bpak-transport-meta           internal_meta
 *
 *
 */

/* BPAK Header
 *
 * magic       Magic number that uniqely identifies the header
 * no_of_tags  How many tags follow the header
 *
 */

struct bpak
{
    uint32_t magic;
    uint32_t no_of_tags;
    uint8_t rz[8]; /* Reserved, set to 0 */
} __attribute__ ((packed));

/* Data within this tag is not included in hashing context */
#define BPAK_FLAG_EXCLUDE_FROM_HASH (1 << 0)

/* Special internal tag for in-flight images */
#define BPAK_FLAG_TRANSPORT_METADATA (1 << 1)

#define BPAK_FLAG_RZ2 (1 << 2)
#define BPAK_FLAG_RZ3 (1 << 3)
#define BPAK_FLAG_RZ4 (1 << 4)
#define BPAK_FLAG_RZ5 (1 << 5)
#define BPAK_FLAG_RZ6 (1 << 6)
#define BPAK_FLAG_RZ7 (1 << 7)

struct bpak_tag
{
    uint32_t magic;
    uint32_t tag;
    uint64_t size;
    uint64_t offset;
    uint8_t flags;
    uint8_t rz[7];           /* Reserved, set to 0 */
} __attribute__ ((packed));

/* Data within this tag is LZ4 compressed  */
#define BPAK_TRANSPORT_FLAG_LZ4_COMPRESSED (1 << 0)

/* Data within this tag is a binary detools patch */
#define BPAK_TRANSPORT_FLAG_DETOOLS_PATCH (1 << 1)

/* Data within this tag has not changed and is not included,
 *   copy existing data */
#define BPAK_TRANSPORT_FLAG_NO_CHANGE (1 << 2)

/* Data within this tag is not included and must be generated by
 *   recipient */
#define BPAK_TRANSPORT_FLAG_GENERATE (1 << 3)

struct bpak_transport_meta
{
    uint32_t tag_ref;
    uint64_t size;
    uint32_t flags;
    uint8_t rz[8]; /* Reserved, set to 0 */
} __attribute__ ((packed));

#endif  // INCLUDE_BPAK_BPAK_H_
