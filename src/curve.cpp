/*
Copyright 2020 Dennis Rohde

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "curve.hpp"

Curve::Curve(const Points &points, const dimensions_t dimensions) : 
    points(points), number_dimensions(dimensions) {
    if (points.empty()) { 
        std::cerr << "warning: constructed empty curve" << std::endl;
        return; 
    }

    #if DEBUG
    std::cout << "constructed curve of complexity " << points.size() << std::endl;
    #endif
}

Curve::Curve(const np::ndarray &in): number_dimensions(in.get_nd()) {
    if (number_dimensions > 2){
        std::cerr << "A Curve requires a 1- or 2-dimensional numpy array of type double."<< std::endl;
        std::cerr << "Current dimensions: " << number_dimensions << std::endl;
        std::cerr << "Current type: " << p::extract<char const*>(p::str(in.get_dtype())) << std::endl;
        std::cerr << "WARNING: constructed empty curve" << std::endl;
        return;
    }
    if (in.get_dtype() != np::dtype::get_builtin<coordinate_t>()) {
        std::cerr << "A Polygonal_Curve requires a 1- or 2-dimensional numpy array of type double."<< std::endl;
        std::cerr << "Current dimensions: " << number_dimensions << std::endl;
        std::cerr << "Current type: " << p::extract<char const*>(p::str(in.get_dtype())) << std::endl;
        std::cerr << "WARNING: constructed empty curve" << std::endl;
        return;
    }
    auto number_points = in.shape(0);
    if (number_dimensions == 2) {
        auto point_size = in.shape(1);
        
        #if DEBUG
        std::cout << "constructing curve of size " << number_points << " and " << point_size << " dimensions" << std::endl;
        #endif
        
        auto strides0 = in.strides(0) / sizeof(coordinate_t);
        auto strides1 = in.strides(1) / sizeof(coordinate_t);
        
        auto data = reinterpret_cast<const coordinate_t*>(in.get_data());
        
        points = Points(number_points);
        
        for (index_t i = 0; i < number_points; ++i, data += strides0) {
            points[i] = Point(std::vector<coordinate_t>(point_size));
            
            auto coord_data = data;
            
            for(index_t j = 0; j < point_size; ++j, coord_data += strides1){
              points[i][j] = *coord_data;
            }
        }
    } else {
        auto strides0 = in.strides(0) / sizeof(coordinate_t);
        
        auto data = reinterpret_cast<const coordinate_t*>(in.get_data());
        
        points = Points(number_points);
        
        for (index_t i = 0; i < number_points; ++i, data += strides0) {
            points[i] = Point(std::vector<coordinate_t>(1));
            
            points[i][0] = *data;
        }
    }
    
    if (points.empty()) { 
        std::cerr << "WARNING: constructed empty curve" << std::endl;
    return; 
    }
}

std::string Curve::str() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::string Curves::str() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream &out, const Curve &curve) {
    out << "[";
    
    for (curve_size_t i = 0; i < curve.size() - 1; ++i) {
        out << curve[i] << ", ";
    }
    
    out << curve[curve.size() -1] << "]";

    return out;
}

std::ostream& operator<<(std::ostream &out, const Curves &curves) {
    out << "{";
    
    for (curve_size_t i = 0; i < curves.size() - 1; ++i) {
        out << curves[i] << ", ";
    }
    
    out << curves[curves.size() -1] << "}";

    return out;
}
