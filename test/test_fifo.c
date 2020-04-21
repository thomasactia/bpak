#include <string.h>
#include <bpak/bpak.h>
#include <bpak/utils.h>
#include <bpak/fifo.h>
#include "nala.h"

TEST(create_fifo)
{
    int rc;
    struct bpak_io *f = NULL;

    rc = bpak_io_fifo_init(&f, 16);
    ASSERT_EQ(rc, BPAK_OK);

    rc = bpak_io_close(f);
    ASSERT_EQ(rc, BPAK_OK);
}

TEST(fifo_write1)
{
    int rc;
    struct bpak_io *f = NULL;
    struct bpak_io_fifo *fifo_p;
    char bfr[16];
    size_t bytes = 0;


    memset(bfr, 0, sizeof(bfr));

    rc = bpak_io_fifo_init(&f, 16);
    ASSERT_EQ(rc, BPAK_OK);

    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 5);

    bytes = bpak_io_read(f, bfr, sizeof(bfr));
    ASSERT_EQ(bytes, 5);
    ASSERT_EQ((char *) bfr, "Hello");
    ASSERT_EQ(fifo_p->tail, 5);

    rc = bpak_io_close(f);
    ASSERT_EQ(rc, BPAK_OK);
}

/*
 *
 * Bfr 01234567
 *
 * W1  Hello
 *     t    h
 *
 * R1       t
 *          h
 * Bfr 01234567
 *
 * W2  lo   Hel
 *       h  t
 * R2
 *       t
 *       h
 *
 * Bfr 01234567
 * W3    Hello
 *       t    h
 */

TEST(fifo_write_overlap)
{
    int rc;
    struct bpak_io *f = NULL;
    struct bpak_io_fifo *fifo_p;
#define BFR_SZ 8
    char *bfr = malloc(8);
    size_t bytes = 0;


    memset(bfr, 0, BFR_SZ);

    rc = bpak_io_fifo_init(&f, BFR_SZ);
    ASSERT_EQ(rc, BPAK_OK);

    /* Write 1 */
    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 5);

    bytes = bpak_io_read(f, bfr, BFR_SZ);
    ASSERT_EQ(bytes, 5);
    ASSERT_EQ((char *) bfr, "Hello");
    ASSERT_EQ(fifo_p->tail, 5);
    ASSERT_EQ(fifo_p->head, 5);

    /* Write 2 */
    memset(bfr, 0, BFR_SZ);
    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 2);
    ASSERT_EQ(fifo_p->tail, 5);

    bytes = bpak_io_read(f, bfr, BFR_SZ);
    ASSERT_EQ(bytes, 5);
    ASSERT_EQ((char *) bfr, "Hello");
    ASSERT_EQ(fifo_p->tail, 2);
    ASSERT_EQ(fifo_p->head, 2);

    /* Write 3 */
    memset(bfr, 0, BFR_SZ);
    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 7);
    ASSERT_EQ(fifo_p->tail, 2);

    bytes = bpak_io_read(f, bfr, BFR_SZ);
    ASSERT_EQ(bytes, 5);
    ASSERT_EQ((char *) bfr, "Hello");
    ASSERT_EQ(fifo_p->tail, 7);
    ASSERT_EQ(fifo_p->head, 7);

    rc = bpak_io_close(f);
    ASSERT_EQ(rc, BPAK_OK);

    free(bfr);
}


/*
 *
 * Bfr 01234567
 *
 * W1  Hello
 *     t    h
 *
 * R1       t
 *          h
 * Bfr 01234567
 *
 * W2  lo   Hel
 *       h  t
 * R2(3)
 *     t
 *       h
 *
 *
 */
TEST(fifo_write_overlap2)
{
    int rc;
    struct bpak_io *f = NULL;
    struct bpak_io_fifo *fifo_p;
#define BFR_SZ 8
    char *bfr = malloc(8);
    size_t bytes = 0;


    memset(bfr, 0, BFR_SZ);

    rc = bpak_io_fifo_init(&f, BFR_SZ);
    ASSERT_EQ(rc, BPAK_OK);

    /* Write 1 */
    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 5);

    bytes = bpak_io_read(f, bfr, BFR_SZ);
    ASSERT_EQ(bytes, 5);
    ASSERT_EQ((char *) bfr, "Hello");
    ASSERT_EQ(fifo_p->tail, 5);
    ASSERT_EQ(fifo_p->head, 5);

    /* Write 2 */
    memset(bfr, 0, BFR_SZ);
    bytes = bpak_io_write(f, "Hello", 5);
    ASSERT_EQ(bytes, 5);

    fifo_p = GET_FIFO_CTX(f);
    ASSERT_EQ(fifo_p->head, 2);
    ASSERT_EQ(fifo_p->tail, 5);

    printf("Read less than what's available\n");
    char out_bfr[3];

    printf("&out_bfr[0] = %p\n", &out_bfr[0]);
    printf("&out_bfr[2] = %p\n", &out_bfr[2]);

    bytes = bpak_io_read(f, out_bfr, 3);
    ASSERT_EQ(bytes, 3);
    ASSERT(strncmp(out_bfr, "Hel", 3) == 0);
    ASSERT_EQ(fifo_p->tail, 0);
    ASSERT_EQ(fifo_p->head, 2);


    rc = bpak_io_close(f);
    ASSERT_EQ(rc, BPAK_OK);

    free(bfr);
}
