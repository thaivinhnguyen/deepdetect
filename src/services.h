/**
 * DeepDetect
 * Copyright (c) 2014 Emmanuel Benazera
 * Author: Emmanuel Benazera <beniz@droidnik.fr>
 *
 * This file is part of deepdetect.
 *
 * deepdetect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * deepdetect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with deepdetect.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SERVICES_H
#define SERVICES_H

#include "utils/variant.hpp"
#include "mlservice.h"
#include "apidata.h"
#include "inputconnectorstrategy.h"
#include "imginputfileconn.h"
#include "outputconnectorstrategy.h"
#include "caffelib.h"
#include <vector>
#include <iostream>

namespace dd
{
  typedef mapbox::util::variant<MLService<CaffeLib,ImgInputFileConn,SupervisedOutput,CaffeModel>> mls_variant_type;
  
  class output
  {
  public:
    output(const int &status, const std::string out)
      :_status(status),_out(out)
    {}
    ~output() 
      {}
    
    int _status = 0;
    std::string _out;
  };

  class visitor_predict : public mapbox::util::static_visitor<output>
  {
  public:
    visitor_predict() {}
    ~visitor_predict() {}
    
    template<typename T>
      output operator() (T &mllib)
      {
        int r = mllib.predict(_ad,_out);
	return output(r,_out);
      }
    
    APIData _ad;
    std::string _out;
  };

  class visitor_train : public mapbox::util::static_visitor<output>
  {
  public:
    visitor_train() {}
    ~visitor_train() {}
    
    template<typename T>
      output operator() (T &mllib)
      {
        int r = mllib.train(_ad,_out);
	return output(r,_out);
      }
    
    APIData _ad;
    std::string _out;
  };
  
  class Services
  {
  public:
    Services() {};
    ~Services() {};
    
    void add_service(mls_variant_type &&mls) 
    {
      _mlservices.push_back(std::move(mls)); 
    }

    int train(const APIData &ad, const int &pos, std::string &out)
    {
      visitor_train vt;
      vt._ad = ad;
      output pout = mapbox::util::apply_visitor(vt,_mlservices.at(pos));
      out = pout._out;
      return pout._status;
    }
    
    int predict(const APIData &ad, const int &pos, std::string &out)
    {
      visitor_predict vp;
      vp._ad = ad;
      output pout = mapbox::util::apply_visitor(vp,_mlservices.at(pos));
      out = pout._out;
      return pout._status;
    }

    std::vector<mls_variant_type> _mlservices;
  };
  
}

#endif
