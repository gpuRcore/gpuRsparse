#pragma once
#ifndef VCL_SP_MAT_HPP
#define VCL_SP_MAT_HPP

// Use OpenCL with ViennaCL
#define VIENNACL_WITH_OPENCL 1

// Use ViennaCL algorithms on Eigen objects
#define VIENNACL_WITH_EIGEN 1

#include <complex>

// ViennaCL headers
#include "viennacl/ocl/device.hpp"
#include "viennacl/ocl/platform.hpp"
#include "viennacl/ocl/backend.hpp"
#include "viennacl/compressed_matrix.hpp"
#include "viennacl/matrix_proxy.hpp"

#include <RcppEigen.h>

#include <typeinfo>
#include "cxxabi.h"
#include <type_traits>
#include <memory>

std::string demangle(const char* name) {

  int status = -4; // some arbitrary value to eliminate the compiler warning

  // enable c++11 by passing the flag -std=c++11 to g++
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, NULL, NULL, &status),
    std::free
  };

  return (status==0) ? res.get() : name ;
}

template <class T>
class vclSpMat {

  static_assert(std::is_same<T, double>::value ||
                std::is_same<T, float>::value,
                "some meaningful error message");

  private:
    int nr, nc;
    Rcpp::StringVector _colNames, _rowNames;
    viennacl::range row_r;
    viennacl::range col_r;
    // viennacl::compressed_matrix<T> *ptr;
    std::shared_ptr<viennacl::compressed_matrix<T> > shptr;
    // = std::make_shared<viennacl::compressed_matrix<T> >();
    viennacl::context ctx;

  public:
    // viennacl::compressed_matrix<T> A;

    // destructor
    ~ vclSpMat() {
      this->release_device();
    }

    vclSpMat() {

      viennacl::range temp_rr(0, 0);
      viennacl::range temp_cr(0, 0);

      row_r = temp_rr;
      col_r = temp_cr;

    } // private default constructor
    vclSpMat(viennacl::compressed_matrix<T> mat, int ctx_id){

      // explicitly pull context for thread safe forking
      ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));

      // must explicity switch context to make sure the same
      // it appears when class initialized the A is set to current context (may not be desired)
      // A.switch_memory_context(ctx);

      shptr = std::make_shared<viennacl::compressed_matrix<T> >(mat);
      // viennacl::compressed_matrix<T> A = mat;

      nr = shptr->size1();
      nc = shptr->size2();
      // ptr = &A;

      // shptr.reset(ptr);
      viennacl::range temp_rr(0, nr);
      viennacl::range temp_cr(0, nc);
      row_r = temp_rr;
      col_r = temp_cr;
    };
    vclSpMat(SEXP A_, int ctx_id){
      // std::cout << "started class" << std::endl;
      Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> Am = Rcpp::as<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> >(A_);
      // std::cout << "dense created" << std::endl;
      Eigen::SparseMatrix<T, Eigen::RowMajor> spAm = Am.sparseView();
      // std::cout << "sparse created" << std::endl;

      // std::cout << spAm << std::endl;

      int K = Am.rows();
      int M = Am.cols();

      // explicitly pull context for thread safe forking
      ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));

      viennacl::compressed_matrix<T> A = viennacl::compressed_matrix<T>(K,M, ctx);
      // shptr = std::make_shared<viennacl::compressed_matrix<T> >(viennacl::compressed_matrix<T>(K,M, ctx = ctx));
      // std::cout << "vcl sparse created" << std::endl;

      // std::cout << demangle(typeid(A).name()) << std::endl;
      // std::cout << demangle(typeid(*shptr).name()) << std::endl;

      viennacl::copy(spAm, A);
      // viennacl::copy(spAm, shptr);
      // viennacl::copy(spAm, *shptr);
      // viennacl::copy(spAm, static_cast<viennacl::compressed_matrix<T> >(*shptr));
      // viennacl::copy(spAm, *std::dynamic_pointer_cast<viennacl::compressed_matrix<T> >(shptr));

      shptr = std::make_shared<viennacl::compressed_matrix<T> >(A);
      // std::cout << "data copied" << std::endl;

      nr = K;
      nc = M;
      // ptr = &A;

      // shptr.reset(ptr);
      viennacl::range temp_rr(0, nr);
      viennacl::range temp_cr(0, nc);
      row_r = temp_rr;
      col_r = temp_cr;
    };
    vclSpMat(
      Eigen::SparseMatrix<T, Eigen::RowMajor> Am,
      int nr_in, int nc_in, int ctx_id
    ){

      // explicitly pull context for thread safe forking
      ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));

      // A.switch_memory_context(ctx);
      // viennacl::compressed_matrix<T> A = viennacl::compressed_matrix<T>(nr_in, nc_in, ctx);
      shptr = std::make_shared<viennacl::compressed_matrix<T> >(viennacl::compressed_matrix<T>(nr_in, nc_in, ctx = ctx));
      viennacl::copy(Am, *shptr);

      nr = nr_in;
      nc = nc_in;
      // ptr = &A;

      // shptr.reset(ptr);
      viennacl::range temp_rr(0, nr);
      viennacl::range temp_cr(0, nc);
      row_r = temp_rr;
      col_r = temp_cr;
    };
    vclSpMat(int nr_in, int nc_in, int ctx_id){

      // explicitly pull context for thread safe forking
      ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));

      // A.switch_memory_context(ctx);
      // viennacl::compressed_matrix<T> A = viennacl::zero_matrix<T>(nr_in, nc_in, ctx);
      shptr = std::make_shared<viennacl::compressed_matrix<T> >(viennacl::zero_matrix<T>(nr_in, nc_in, ctx));

      nr = nr_in;
      nc = nc_in;
      // ptr = &A;

      // shptr.reset(ptr);
      viennacl::range temp_rr(0, nr);
      viennacl::range temp_cr(0, nc);
      row_r = temp_rr;
      col_r = temp_cr;
    };
    vclSpMat(int nr_in, int nc_in, T scalar, int ctx_id){

      // explicitly pull context for thread safe forking
      ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));

      // A.switch_memory_context(ctx);
      // viennacl::compressed_matrix<T> A = viennacl::scalar_matrix<T>(nr_in, nc_in, scalar, ctx);
      shptr = std::make_shared<viennacl::compressed_matrix<T> >(viennacl::scalar_matrix<T>(nr_in, nc_in, scalar, ctx));

      nr = nr_in;
      nc = nc_in;
      // ptr = &A;

      // shptr.reset(ptr);
      viennacl::range temp_rr(0, nr);
      viennacl::range temp_cr(0, nc);
      row_r = temp_rr;
      col_r = temp_cr;
    };
    // vclSpMat(Rcpp::XPtr<vclSpMat<T> > dynMat)        {
    //     nr = dynMat->nrow();
    //     nc = dynMat->ncol();
    //     row_r = dynMat->row_range();
    //     col_r = dynMat->col_range();
    //     ptr = dynMat->getPtr();
    // };


    viennacl::compressed_matrix<T>* getPtr(){
      return shptr.get();
    };
    std::shared_ptr<viennacl::compressed_matrix<T> > sharedPtr(){
      return shptr;
    };
    std::shared_ptr<viennacl::matrix_range<viennacl::compressed_matrix<T> > > sharedBlockPtr(){
      viennacl::matrix_range<viennacl::compressed_matrix<T> > m_sub(*shptr.get(), row_r, col_r);

      std::shared_ptr<viennacl::matrix_range<viennacl::compressed_matrix<T> > > block_shptr = std::make_shared<viennacl::matrix_range<viennacl::compressed_matrix<T> > >(m_sub);
      return block_shptr;
      // return shptr;
    };

    int nrow() { return nr; }
    int ncol() { return nc; }
    viennacl::range row_range() { return row_r; }
    viennacl::range col_range() { return col_r; }

    viennacl::context getContext(){
      return ctx;
    }

    void setRange(
        viennacl::range row_in,
        viennacl::range col_in
    ){
      row_r = row_in;
      col_r = col_in;
    }
    void setRange(
        int row_start, int row_end,
        int col_start, int col_end
    ){
      if(row_r.size() == 0 && col_r.size() == 0){
        viennacl::range temp_rr(row_start, row_end);
        viennacl::range temp_cr(col_start, col_end);

        row_r = temp_rr;
        col_r = temp_cr;
      }else{
        viennacl::range temp_rr(row_start + row_r.start(), row_end + row_r.start());
        viennacl::range temp_cr(col_start + col_r.start(), col_end + col_r.start());

        row_r = temp_rr;
        col_r = temp_cr;
      }
    };
    void setMatrix(viennacl::compressed_matrix<T> mat){
      viennacl::compressed_matrix<T> A = mat;
      // ptr = &A;
      shptr = std::make_shared<viennacl::compressed_matrix<T> >(A);
      // shptr.reset(ptr);
    }
    void updateMatrix(const viennacl::compressed_matrix<T> &mat){
      viennacl::matrix_range<viennacl::compressed_matrix<T> > m_sub(*shptr.get(), row_r, col_r);
      m_sub = mat;
    }
    void updateMatrix(const viennacl::matrix_range<viennacl::compressed_matrix<T> > &mat){
      viennacl::matrix_range<viennacl::compressed_matrix<T> > m_sub(*shptr.get(), row_r, col_r);
      m_sub = mat;
    }
    // void setMatrix(viennacl::matrix_range<viennacl::compressed_matrix<T> > mat){
    //     A = mat;
    //     ptr = &A;
    // };
    // void createMatrix(int nr_in, int nc_in, int ctx_id){
    //
    //     // std::cout << "creating matrix" << std::endl;
    //
    //     viennacl::context ctx;
    //
    //     // explicitly pull context for thread safe forking
    //     ctx = viennacl::context(viennacl::ocl::get_context(static_cast<long>(ctx_id)));
    //
    //     // std::cout << "pulled context" << std::endl;
    //     // std::cout << ctx_id << std::endl;
    //
    //     A = viennacl::compressed_matrix<T>(nr_in, nc_in, ctx=ctx);
    //
    //     // std::cout << "assigned new matrix" << std::endl;
    //
    //     ptr = &A;
    // };
    void setDims(int nr_in, int nc_in){
      nr = nr_in;
      nc = nc_in;
    }

    void setColumnNames(Rcpp::StringVector names){
      _colNames = names;
    }
    Rcpp::StringVector getColumnNames(){
      return _colNames;
    }

    void setPtr(viennacl::compressed_matrix<T>* ptr_){
      // ptr = ptr_;
      shptr = std::make_shared<viennacl::compressed_matrix<T> >(*ptr_);
      // shptr.reset(ptr_);
    };
    void setSharedPtr(std::shared_ptr<viennacl::compressed_matrix<T> > shptr_){
      shptr = shptr_;
    };

    viennacl::matrix_range<viennacl::compressed_matrix<T> > data(){
      viennacl::matrix_range<viennacl::compressed_matrix<T> > m_sub(*shptr.get(), row_r, col_r);
      return m_sub;
    };
    viennacl::compressed_matrix<T> matrix() {
      return *shptr;
    }

    viennacl::vector<T> row(int row_id) {
      // always refer to the block
      viennacl::matrix_range<viennacl::compressed_matrix<T> > m_sub(*shptr.get(), row_r, col_r);

      // return the desired row
      return viennacl::row(m_sub, row_id);
    }


    // release device memory
    void release_device(){
      shptr.reset();
    };
};

#endif
