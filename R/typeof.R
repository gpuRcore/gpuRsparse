#' @title Get gpuRsparse object type
#' @description \code{typeof} determines the type (i.e. storage mode) of a
#' gpuRsparse object
#' @param x A gpuRsparse object
#' @rdname typeof-gpuRsparse-methods
#' @author Charles Determan Jr.
#' @import methods
#' @export
setMethod('typeof', signature(x="vclSparseMatrix"),
          function(x) {
            switch(class(x),
                   "fvclSparseMatrix" = "float",
                   "dvclSparseMatrix" = "double",
                   "fvclSparseMatrixBlock" = "float",
                   "dvclSparseMatrixBlock" = "double",
                   stop("unrecognized class"))
          })
