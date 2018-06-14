
library(gpuRsparse)

context("CPU vclSparseMatrix classes")

current_context <- set_device_context("cpu")

set.seed(123)
A <- matrix(seq.int(100), 100)
D <- matrix(rnorm(16), 4)

print(showMethods("typeof"))

# test_that("CPU vclMatrix float class initializer" ,{
#
#   has_cpu_skip()
#
#   vclD <- vclSparseMatrix(D, type="float")
#
#   expect_is(vclD, "fvclSparseMatrix")
#   # expect_equal(vclD[], D, tolerance=1e-07,
#   #              info="vcl float matrix elements not equivalent")
#   # expect_equal(dim(vclD), dim(D))
#   # expect_equal(ncol(vclD), ncol(D))
#   # expect_equal(nrow(vclD), nrow(D))
#   expect_equal(typeof(vclD), "float")
# })

test_that("CPU vclSparseMatrix double class initializer" ,{

  has_cpu_skip()

  print(showMethods("typeof"))

  vclD <- vclSparseMatrix(D, type = "double")

  print(typeof(vclD))

  expect_is(vclD, "dvclSparseMatrix")
  # expect_equal(vclD[], D, tolerance=.Machine$double.eps ^ 0.5,
  #              info="vcl double matrix elements not equivalent")
  # expect_equal(dim(vclD), dim(D))
  # expect_equal(ncol(vclD), ncol(D))
  # expect_equal(nrow(vclD), nrow(D))
  expect_equal(typeof(vclD), "double")
})
