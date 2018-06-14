
#' @title Construct a vclSparseMatrix
#' @description Construct a vclSparseMatrix of a class that inherits
#' from \code{vclSparseMatrix}.  This class points to memory directly on
#' the GPU to avoid the cost of data transfer between host and device.
#' @param data An object that is or can be converted to a
#' \code{matrix} object
#' @param nrow An integer specifying the number of rows
#' @param ncol An integer specifying the number of columns
#' @param type A character string specifying the type of vclSparseMatrix.  Default
#' is NULL where type is inherited from the source data type.
#' @param ctx_id An integer specifying the object's context
#' @param ... Additional method to pass to vclSparseMatrix methods
#' @return A vclSparseMatrix object
#' @docType methods
#' @rdname vclSparseMatrix-methods
#' @author Charles Determan Jr.
#' @export
setGeneric("vclSparseMatrix", function(data = NA, nrow=NA, ncol=NA, type=NULL, ...){
  standardGeneric("vclSparseMatrix")
})

#' @rdname vclSparseMatrix-methods
#' @aliases vclSparseMatrix,matrix
setMethod('vclSparseMatrix',
          signature(data = 'matrix'),
          function(data, type=NULL, ctx_id=NULL){

            if (is.null(type)){
              type <- getOption("gpuR.default.type")
            }

            device <- if(is.null(ctx_id)) currentDevice() else listContexts()[ctx_id,]

            context_index <- ifelse(is.null(ctx_id), currentContext(), as.integer(ctx_id))
            device_index <- if(is.null(ctx_id)) as.integer(device$device_index) else device$device_index + 1L

            platform_index <- if(is.null(ctx_id)) currentPlatform()$platform_index else device$platform_index + 1L
            platform_name <- platformInfo(platform_index)$platformName

            device_type <- device$device_type
            device_name <- switch(device_type,
                                  "gpu" = gpuInfo(
                                    device_idx = as.integer(device_index),
                                    context_idx = context_index)$deviceName,
                                  "cpu" = cpuInfo(
                                    device_idx = as.integer(device_index),
                                    context_idx = context_index)$deviceName,
                                  stop("Unrecognized device type")
            )

            data = switch(type,
                          float = {
                            new("fvclSparseMatrix",
                                address=cpp_sexp_mat_to_vcl_sp_mat(data, 6L, context_index - 1),
                                .context_index = context_index,
                                .platform_index = platform_index,
                                .platform = platform_name,
                                .device_index = device_index,
                                .device = device_name)
                          },
                          double = {
                            assert_has_double(device_index, context_index)
                            new("dvclSparseMatrix",
                                address = cpp_sexp_mat_to_vcl_sp_mat(data, 8L, context_index - 1),
                                .context_index = context_index,
                                .platform_index = platform_index,
                                .platform = platform_name,
                                .device_index = device_index,
                                .device = device_name)
                          },
                          stop("this is an unrecognized
                                 or unimplemented data type")
            )
            return(data)
          },
          valueClass = "vclSparseMatrix")
