
#include "gpuRsparse/vcl_sp_mat.hpp"

using namespace Rcpp;

// convert SEXP Matrix to ViennaCL Sparse matrix
template <typename T>
SEXP cpp_sexp_mat_to_vcl_sp_mat(
    SEXP A,
    int ctx_id)
{
  vclSpMat<T> *mat = new vclSpMat<T>(A, ctx_id);
  Rcpp::XPtr<vclSpMat<T> > pMat(mat);

  return pMat;
}

// convert XPtr ViennaCL Sparse Matrix to SEXP matrix
template <typename T>
SEXP
sp_vcl_to_sexp(SEXP A)
{
  Rcpp::XPtr<vclSpMat<T> > ptrA(A);
  // viennacl::matrix_range<viennacl::compressed_matrix<T> > tempA = ptrA->data();

  // viennacl::compressed_matrix<T> pA = static_cast<viennacl::compressed_matrix<T> >(tempA);
  std::shared_ptr<viennacl::compressed_matrix<T> > pA = ptrA->sharedPtr();
  int nr = pA->size1();
  int nc = pA->size2();

  Eigen::SparseMatrix<T, Eigen::RowMajor> Am(nr, nc);

  viennacl::copy(*pA, Am);

  return wrap(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>(Am));
}


// [[Rcpp::export]]
SEXP
cpp_sexp_mat_to_vcl_sp_mat(
  SEXP ptrA,
  int type_flag,
  int ctx_id)
{
  switch(type_flag) {
  case 6:
    return cpp_sexp_mat_to_vcl_sp_mat<float>(ptrA, ctx_id);
  case 8:
    return cpp_sexp_mat_to_vcl_sp_mat<double>(ptrA, ctx_id);
  default:
    throw Rcpp::exception("unknown type detected for vclSparseMatrix object!");
  }
}


// [[Rcpp::export]]
SEXP
  sp_vcl_to_sexp(
    SEXP ptrA,
    int type_flag,
    int ctx_id)
  {
    switch(type_flag) {
    case 6:
      return sp_vcl_to_sexp<float>(ptrA);
    case 8:
      return sp_vcl_to_sexp<double>(ptrA);
    default:
      throw Rcpp::exception("unknown type detected for vclSparseMatrix object!");
    }
  }


