#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/text_format.h>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <lmdb.h>

#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>

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
                int percent)
{
    scoped_ptr<db::DB> db(db::GetDB("lmdb"));
    std::ifstream isdata(data_file, std::ios::in);
    int  lineno = 0;

    db->Open(db_path, db::NEW);
    scoped_ptr<db::Transaction> txn(db->NewTransaction());

    srandom(time(NULL));

    while (!isdata.eof()) {
        string value;
        Datum datum;
        double data[64];
        int    nfields, s;
        char   label;

        isdata >> value;
        
        parse_line(value, &nfields, &data[0]);
        std::cout << "nfields " << nfields << std::endl;

        if ( nfields <= 0) {
            break;
        }

        datum.set_channels(1);
        datum.set_height(nfields-1);
        datum.set_width(1);
        datum.set_data(data, sizeof(data[0])*(nfields-1));

        s = random() % 100;
        if ( s < percent) {
            label = 'v';
        }
        else {
            label = 't';
        }
        datum.set_label((int)data[nfields-1]);
        lineno ++;

        string key_str = caffe::format_int(lineno,8);
        datum.SerializeToString(&value);

        txn->Put(key_str, value);
        if ( lineno % 1000 == 0) {
            txn->Commit();
        }
    }

    if (lineno % 1000 != 0) {
        txn->Commit();
    }

    isdata.close();

    db->Close();

    return 0;
}
int
main(int argc, char *argv[])
{
    char *datafile="data.dat";
    char *dbpath="./data";
    int   percent=5;

    if ( argc >= 2) {
        datafile=argv[1];
    }

    if (argc >=3 ) {
        dbpath = argv[2];
    }

    if (argc >=4 ) {
        percent=atoi(argv[3]);
    }

    convert_dataset(datafile, dbpath, percent);

    return 0;
}
