//
// Created by den on 05.07.18.
//

#pragma once

#include <memory>
#include "dgn/InputStream.hpp"
#include "dwfcore/InputStream.h"
namespace c60 {

/**получает данные из Java потока**/
		class DwfJavaInputStream : public DWFCore::DWFInputStream {
		protected:
			  std::shared_ptr<dgn::InputStream> javaStream;

		public:

				DwfJavaInputStream(std::shared_ptr<dgn::InputStream>javaStream);

				size_t available() const throw(DWFCore::DWFException) override;

				size_t read(void *pBuffer, size_t nBytesToRead) throw(DWFCore::DWFException) override;

				off_t seek(int eOrigin, off_t nOffset) throw(DWFCore::DWFException) override;
		};

}
