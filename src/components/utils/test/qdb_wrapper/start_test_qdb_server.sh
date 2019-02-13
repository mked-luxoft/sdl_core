# This script star QDB server for SDL
# Need superuser to start qdb
DB_RESUMPTION_PATH=$PWD/resumption
DB_POLICY_PATH=$PWD/policy
DB_BACKUP_PATH=$PWD/backup
DB_TEST_PATH=$PDW/test_database
# Creating storage for the resumption
if [ ! -d $DB_RESUMPTION_PATH ] ; then
   mkdir -p $DB_RESUMPTION_PATH
fi
# Creating storage for the backup
if [ ! -d $DB_BACKUP_PATH ] ; then
   mkdir -p $DB_BACKUP_PATH
fi
# Creating storage for the policy
if [ ! -d $DB_POLICY_PATH ] ; then
   mkdir -p $DB_POLICY_PATH
fi
# Creating storage for the test
if [ ! -d $DB_TEST_PATH ] ; then
   mkdir -p $DB_TEST_PATH
fi
# Run PPS service
if ! pidin | grep pps > /dev/null
then
   if [ -d /pps ] ; then
   	  rm -r /pps
   fi
   pps
fi
# Checking PPS configuration path
if [ ! -d /pps/qnx/qdb/config ] ; then
   mkdir -p /pps/qnx/qdb/config
   mkdir -p /pps/qnx/qdb/status
fi
# Config init for resumption and policy &  Run QDB
# If no tempstore option (-o tempstore) is specified on the command line,
# the QDB uses the environment variable TMPDIR to obtain the location it should use for temporary storage.
# If all the above conditions are met, the QDB sets the internal temporary storage to the value of TMPDIR.
# If any of the above conditions are not met, the QDB logs errors to the slog and fails to start up.
echo Filename::$DB_RESUMPTION_PATH/resumption > /pps/qnx/qdb/config/resumption
echo BackupDir::$DB_BACKUP_PATH >> /pps/qnx/qdb/config/resumption
echo Filename::$DB_POLICY_PATH/policy > /pps/qnx/qdb/config/policy
echo Filename::$DB_TEST_PATH/test_database > /pps/qnx/qdb/config/test_database
if ! pidin | grep qdb > /dev/null
then
   qdb -Otempstore=/tmp
fi
# Since there is no guaranteed init order for QDB, we have to wait for it
if [ ! -d /dev/qdb ] ; then
   waitfor /dev/qdb
fi
