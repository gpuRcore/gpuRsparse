# The primary class for all vclSparseMatrix objects

#' @title vclSparseMatrix Class
#' @description This is the 'mother' class for all
#' vclSparseMatrix objects.  These objects are pointers
#' to viennacl compressed matrices directly on the GPU.  This will
#' avoid the overhead of passing data back and forth
#' between the host and device.
#'
#' As such, any changes made
#' to normal R 'copies' (e.g. A <- B) will be propogated to
#' the parent object.
#'
#' There are multiple child classes that correspond
#' to the particular data type contained.  These include
#' \code{fvclSparseMatrix} and
#' \code{dvclSparseMatrix} corresponding to integer, float, and
#' double data types respectively.
#' @section Slots:
#'  Common to all vclSparseMatrix objects in the package
#'  \describe{
#'      \item{\code{address}:}{Pointer to data matrix}
#'      \item{\code{.context_index}:}{Integer index of OpenCL contexts}
#'      \item{\code{.platform_index}:}{Integer index of OpenCL platforms}
#'      \item{\code{.platform}:}{Name of OpenCL platform}
#'      \item{\code{.device_index}:}{Integer index of active device}
#'      \item{\code{.device}:}{Name of active device}
#'  }
#' @note R does not contain a native float type.  As such,
#' the matrix data within a \code{\link{fvclSparseMatrix-class}}
#' will be represented as double but downcast when any
#' vclSparseMatrix methods are used.
#'
#' May also remove the type slot
#'
#' @name vclSparseMatrix-class
#' @rdname vclSparseMatrix-class
#' @author Charles Determan Jr.
#' @seealso \code{\link{fvclSparseMatrix-class}},
#' \code{\link{dvclSparseMatrix-class}}
#' @export
setClass('vclSparseMatrix',
         slots = c(address="externalptr",
                   .context_index = "integer",
                   .platform_index = "integer",
                   .platform = "character",
                   .device_index = "integer",
                   .device = "character"))


#' @title fvclSparseMatrix Class
#' @description An integer type matrix in the S4 \code{vclSparseMatrix}
#' representation.
#' @section Slots:
#'  \describe{
#'      \item{\code{address}:}{Pointer to a float matrix.}
#'  }
#' @name fvclSparseMatrix-class
#' @rdname fvclSparseMatrix-class
#' @author Charles Determan Jr.
#' @seealso \code{\link{vclSparseMatrix-class}},
#' \code{\link{dvclSparseMatrix-class}}
#' @export
setClass("fvclSparseMatrix",
         contains = "vclSparseMatrix",
         validity = function(object) {
           if( typeof(object) != "float"){
             return("fvclSparseMatrix must be of type 'float'")
           }
           TRUE
         })


#' @title dvclSparseMatrix Class
#' @description An integer type matrix in the S4 \code{vclSparseMatrix}
#' representation.
#' @section Slots:
#'  \describe{
#'      \item{\code{address}:}{Pointer to a double type matrix}
#'  }
#' @name dvclSparseMatrix-class
#' @rdname dvclSparseMatrix-class
#' @author Charles Determan Jr.
#' @seealso \code{\link{vclSparseMatrix-class}},
#' \code{\link{fvclSparseMatrix-class}}
#' @export
setClass("dvclSparseMatrix",
         contains = "vclSparseMatrix")


# @export
setClass("fvclSparseMatrixBlock",
         contains = "fvclSparseMatrix")

# @export
setClass("dvclSparseMatrixBlock",
         contains = "dvclSparseMatrix")

