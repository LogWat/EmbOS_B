#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union {
    int value;
    unsigned char image[sizeof(int)];
} lint;

typedef union {
    short value;
    unsigned char image[sizeof(short)];
} lshort;

typedef struct {
    char id[4];
    lint size;
    char type[4];
} riff_chunk __attribute__((packed));

typedef struct {
    char id[4];
    lint size;
    lshort format;
    lshort channels;
    lint sample_rate;
    lint xfer_rate;
    lshort bytes;
    lshort bits;
} fmt_chunk __attribute__((packed));

typedef struct {
    char id[4];
    lint size;
} sub_chunk __attribute__((packed));

riff_chunk riff_hdr;
fmt_chunk fmt_hdr;
sub_chunk sub_hdr;

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int load_little_int(lint *target) {
    int val;
    val = target->image[0];
    val += target->image[1] << 8;
    val += target->image[2] << 16;
    val += target->image[3] << 24;
    return val;
}

int load_little_short(lshort *target) {
    int val;
    val = target->image[0];
    val += target->image[1] << 8;
    return val;
}

int read_riff(riff_chunk *r) {
    if (strncmp(r->id, "RIFF", 4) != 0) {
        die("Not a RIFF file");
    }
    if (strncmp(r->type, "WAVE", 4) != 0) {
        die("Not a WAVE file");
    }
    return load_little_int(&r->size);
}

void read_fmt(fmt_chunk *f, int *size, int *samle, int *bits) {
    if (strncmp(f->id, "fmt ", 4) != 0) {
        die("Not a fmt chunk");
    }
    *size = load_little_int(&f->size);
    if (*size < 16) {
        die("fmt chunk too small");
    }
    if (load_little_short(&f->format) != 1) {
        die("Not a PCM format");
    }
    if (load_little_short(&f->channels) != 1) {
        die("Not a mono file");
    }
    *samle = load_little_int(&f->sample_rate);
    *bits = load_little_short(&f->bits);
    if (*bits != 8) {
        die("Not an 8-bit file");
    }
}


int main(int argc, char **argv) {
    int fd, ret, size, bytes, sample, bits, data;
    void *buf;
    unsigned char *ptr;

    if (argc != 2) {
        die("Usage: wave2gba <file>");
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        die("Could not open file");
    }

    ret = read(fd, &riff_hdr, sizeof(riff_chunk));
    if (ret != sizeof(riff_chunk)) {
        die("Could not read RIFF chunk");
    }
    ret = read_riff(&riff_hdr);

    ret = read(fd, &fmt_hdr, sizeof(fmt_chunk));
    if (ret < 0) {
        die("Could not read RIFF chunk");
    }
    read_fmt(&fmt_hdr, &size, &sample, &bits);
    if (size > (sizeof(fmt_chunk) - 8)) {
        ret = lseek(fd, size - (sizeof(fmt_chunk) - 8), SEEK_CUR);
    }

    while (1) {
        ret = read(fd, &sub_hdr, sizeof(sub_chunk));
        if (ret != sizeof(sub_chunk)) {
            die("Could not read sub chunk");
        }
        if (strncmp(sub_hdr.id, "data", 4) == 0) {
            break;
        }
        bytes = load_little_int(&sub_hdr.size);
        ret = lseek(fd, bytes, SEEK_CUR);
        if (ret < 0) {
            die("Could not seek to data");
        }
    }

    buf = malloc(bytes);
    if (buf == NULL) {
        die("Could not allocate memory");
    }
    ptr = (char *)buf;
    ret = read(fd, buf, bytes);
    close(fd);

    printf("const char wave_comment[] = \"%s\";\n", argv[1]);
    printf("int wave_freq = %d;\n", sample);
    printf("int wave_length = %d;\n", bytes);
    printf("const char wave_data[] = {\n");
    for (int i = 0; i < bytes; i++) {
        data = (*ptr++);
        printf("%d, ", data - 128);
        if ((i % 16) == 15) {
            printf("\n");
        }
    }
    printf("};\n");

    return 0;
}