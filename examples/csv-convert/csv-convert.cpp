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
#include "boost/tokenizer.hpp"
#include "boost/spirit/include/qi.hpp"

#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hpp"

using namespace caffe;
using boost::scoped_ptr;
using std::string;

namespace qi = boost::spirit::qi;

int
parse_line(const std::string s,
           int *nfields,
           double *data)
{
    typedef boost::tokenizer< boost::escaped_list_separator<char> ,
                              std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    Tokenizer tok(s, seps);
    int   i;

    i = 0;

    for (auto v: tok) {
        data[i] = atof(v.c_str());
        i ++;
    }

    *nfields = i;
    return 1;
}

int
convert_dataset(const char *data_file,
                const char *db_path,
                int ncolData,
                int percent)
{
    scoped_ptr<db::DB> db(db::GetDB("lmdb"));
    std::ifstream isdata(data_file, std::ios::in);

    db->Open(db_path, db::NEW);
    scoped_ptr<db::Transaction> txn(db->NewTransaction());

    char label;
    string value;
    Datum  datum;
    int    lineno;
    
    while (!isdata.eof()) {
        double data[64];
        int    nfields;

        isdata >> value;
        
        parse_line(value, &nfields, &data[0]);
        cout << "nfields " << nfields << endl;
    }

    isdata.close();

    return 0;
}
int
main(int argc, char *argv[])
{
    convert_dataset("/tmp/abcd.dat", "/tmp", 20, 5);

    return 0;
}
