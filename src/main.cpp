#include "restart_db.h"
#include "restart_app.h"

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.

    return 0;
}