//
// Created by den on 05.07.18.
//

#pragma once

#include "dgn/InputStream.hpp"
#include "dwfcore/InputStream.h"

/**получает данные из Java потока**/
class DwfJavaInputStream : public DWFCore::DWFInputStream{
		size_t available() const throw( DWFException ) override;


};


