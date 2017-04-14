
#ifndef LINALG_HPP
#define LINALG_HPP

namespace linalg {

// Projects a vector A onto a vector B
template <class DataType, int Size>
Eigen::Matrix<DataType, Size, 1>
project(
		const Eigen::Matrix<DataType, Size, 1> &vectorA,
		const Eigen::Matrix<DataType, Size, 1> &vectorB)
{
	const Eigen::Matrix<DataType, Size, 1> bn = vectorB.normalized();
	return bn.dot(vectorA)*bn;
}

}


#endif // LINALG_HPP
