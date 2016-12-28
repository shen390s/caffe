#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <lmdb.h>

#include <stdint.h>
#include <sys/stat.h>

#include <fstream>
#include <string>

#include "boost/scoped_ptr.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hppp"

using namespace caffe;
using boost::scoped_ptr;
using std::string;

int
main(int argc, char *argv[])
{
    return 0;
}
