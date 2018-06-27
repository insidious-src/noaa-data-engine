#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>

class CSVParser
{
public:
    typedef std::string                string_type;
    typedef std::vector<string_type[]> vector_type;
    typedef std::size_t                size_type  ;

    CSVParser (string_type const& file_path);

    template <std::size_t N>
    string_type get(size_type idx) const
    { return m_data[idx][N]; }

private:
    vector_type m_data;
};

#endif // CSV_H
