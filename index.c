#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int compare_index_entries(const void *a, const void *b) {
    return strcmp(((const IndexEntry *)a)->path,
                  ((const IndexEntry *)b)->path);
}

int index_save(Index *index) {
    char tmp[] = ".pes/index.tmpXXXXXX";
    int fd = mkstemp(tmp);
    if (fd < 0) return -1;

    FILE *fp = fdopen(fd, "w");

    qsort(index->entries, index->count,
          sizeof(IndexEntry), compare_index_entries);

    for (int i = 0; i < index->count; i++) {
        char hex[65];
        hash_to_hex(&index->entries[i].hash, hex);

        fprintf(fp, "%o %s %lu %u %s\n",
                index->entries[i].mode,
                hex,
                index->entries[i].mtime_sec,
                index->entries[i].size,
                index->entries[i].path);
    }

    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    rename(tmp, ".pes/index");
    return 0;
}

// rest unchanged