/*
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sql/sql_query.h"
#include <string.h>
#include <cassert>
#include <algorithm>
#include "sql/sql_database.h"
#include "utils/logger.h"
#include <errno.h>

namespace utils {
namespace dbms {

CREATE_LOGGERPTR_GLOBAL(logger_, "Utils")

class SetBind : public boost::static_visitor<qdb_binding_t> {
 public:
  // In QDB the number of position for binding starts since 1.
  explicit SetBind(const int index) : index_(index + 1) {}

  qdb_binding_t operator()(const int64_t& x) const {
    return SetBindData(QDB_INTEGER, sizeof(x), &x);
  }

  qdb_binding_t operator()(const std::string& x) const {
    return SetBindData(QDB_TEXT, strlen(x.c_str()), x.c_str());
  }

  qdb_binding_t operator()(const double& x) const {
    return SetBindData(QDB_REAL, sizeof(x), &x);
  }

  qdb_binding_t operator()(const void* x) const {
    DCHECK(NULL == x);
    return SetBindData(QDB_NULL, 0, NULL);
  }

 private:
  qdb_binding_t SetBindData(const int type, const int len, const void* data) const {
    qdb_binding_t bind;
    bind.index = index_;
    bind.type = type;
    bind.len = len;
    bind.data = data;
    return bind;
  }

  const int index_;
};

SQLQuery::SQLQuery(SQLDatabase* db)
    : db_(db)
    , query_("")
    , statement_(-1)
    , result_(NULL)
    , current_row_(0)
    , rows_(0)
    , error_(Error::OK) {}

SQLQuery::~SQLQuery() {
  Finalize();
}

bool SQLQuery::Prepare(const std::string& query) {
  Finalize();
  query_ = query;
  statement_ = qdb_stmt_init(db_->conn(), query.c_str(), query.length() + 1);
  if (statement_ == -1) {
    LOG4CXX_DEBUG(logger_, "Prepare error: " << strerror(errno));
    error_ = Error::ERROR;
    return false;
  }
  return true;
}

uint8_t SQLQuery::SetBinds() {
  for (const auto& it : binds_) {
    qdb_bindings_.push_back(boost::apply_visitor(SetBind(it.first), it.second));
  }

  return qdb_bindings_.size();
}

bool SQLQuery::Result() {
  result_ = qdb_getresult(db_->conn());
  if (!result_) {
    error_ = Error::ERROR;
    return false;
  }
  rows_ = qdb_rows(result_);
  if (rows_ == -1) {
    rows_ = 0;
    error_ = Error::ERROR;
    return false;
  }
  return true;
}

bool SQLQuery::Exec() {
  sync_primitives::AutoLock auto_lock(bindings_lock_);
  if (result_)
    return true;

  current_row_ = 0;
  uint8_t binding_count = SetBinds();
  qdb_binding_t* bindings = (qdb_bindings_.empty() ? NULL : &qdb_bindings_[0]);
  if (qdb_stmt_exec(db_->conn(), statement_, bindings, binding_count) == -1) {
    error_ = Error::ERROR;
    return false;
  }
  return Result();
}

bool SQLQuery::Next() {
  ++current_row_;
  return Exec() && current_row_ < rows_;
}

bool SQLQuery::Reset() {
  sync_primitives::AutoLock auto_lock(bindings_lock_);
  qdb_bindings_.clear();
  binds_.clear();
  rows_ = 0;
  current_row_ = 0;
  if (result_ && qdb_freeresult(result_) == -1) {
    error_ = Error::ERROR;
    return false;
  }
  result_ = NULL;
  return true;
}

void SQLQuery::Finalize() {
  if (Reset() && qdb_stmt_free(db_->conn(), statement_) != -1) {
    statement_ = 0;
  } else {
    error_ = Error::ERROR;
  }
}

bool SQLQuery::Exec(const std::string& query) {
  query_ = query;
  if (qdb_statement(db_->conn(), query.c_str()) == -1) {
    error_ = Error::ERROR;
    return false;
  }
  return true;
}

void SQLQuery::Bind(int pos, int value) {
  Bind(pos, static_cast<int64_t>(value));
}

void SQLQuery::Bind(int pos, int64_t value) {
  binds_.push_back({pos, value});
}

void SQLQuery::Bind(int pos, double value) {
  binds_.push_back({pos, value});
}

void SQLQuery::Bind(int pos, bool value) {
  Bind(pos, static_cast<int>(value));
}

void SQLQuery::Bind(int pos, const std::string& value) {
  binds_.push_back({pos, value});
}

void SQLQuery::Bind(int pos) {
  binds_.push_back({pos, nullptr});
}

bool SQLQuery::GetBoolean(int pos) const {
  return static_cast<bool>(GetInteger(pos));
}

int SQLQuery::GetInteger(int pos) const {
  void* ret = qdb_cell(result_, current_row_, pos);
  if (rows_ != 0 && ret) {
    return *static_cast<int*>(ret);
  }
  return 0;
}

uint32_t SQLQuery::GetUInteger(int pos) const {
  void* ret = qdb_cell(result_, current_row_, pos);
  if (rows_ != 0 && ret) {
    return *static_cast<uint32_t*>(ret);
  }
  return 0;
}

int64_t SQLQuery::GetLongInt(int pos) const {
  void* ret = qdb_cell(result_, current_row_, pos);
  if (rows_ != 0 && ret) {
    return *static_cast<int64_t*>(ret);
  }
  return 0;
}

double SQLQuery::GetDouble(int pos) const {
  void* ret = qdb_cell(result_, current_row_, pos);
  if (rows_ != 0 && ret) {
    return *static_cast<double*>(ret);
  }
  return 0;
}

std::string SQLQuery::GetString(int pos) const {
  void* ret = qdb_cell(result_, current_row_, pos);
  if (rows_ != 0 && ret) {
    return static_cast<const char*>(ret);
  }
  return "";
}

bool SQLQuery::IsNull(int pos) const {
  return rows_ == 0 || qdb_cell_type(result_, current_row_, pos) == QDB_NULL;
}

const std::string& SQLQuery::query() const {
  // TODO(KKolodiy): may return string query with value of arguments
  return query_;
}

SQLError SQLQuery::LastError() const {
  return SQLError(error_, qdb_geterrmsg(db_->conn()));
}

int64_t SQLQuery::LastInsertId() const {
  return qdb_last_insert_rowid(db_->conn(), result_);
}

}  // namespace dbms
}  // namespace utils
