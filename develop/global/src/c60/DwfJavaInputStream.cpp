//
// Created by den on 05.07.18.
//

#include "DwfJavaInputStream.h"

namespace c60 {
		DwfJavaInputStream::DwfJavaInputStream(std::shared_ptr<dgn::InputStream> javaStream): DWFCore::DWFInputStream(){
			this->javaStream = javaStream;
		}

		size_t DwfJavaInputStream::available() const throw(DWFCore::DWFException){
			return (size_t)this->javaStream->available();
		}

		size_t DwfJavaInputStream::read(void *pBuffer, size_t nBytesToRead) throw(DWFCore::DWFException){
			const vector<uint8_t> data = this->javaStream->read((int32_t)nBytesToRead);
			std::copy(data.begin(),data.end(), (uint8_t *)pBuffer);
			return data.size();
		}

		off_t  DwfJavaInputStream::seek(int eOrigin, off_t nOffset) throw(DWFCore::DWFException){
			_DWFCORE_THROW( DWFCore::DWFNotImplementedException, /*NOXLATE*/L"This opcode not implemented in java stream.." )
		}
}