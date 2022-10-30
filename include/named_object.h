//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_NAMED_OBJECT_H
#define AX_MEDIAPIPLE_NAMED_OBJECT_H

#include <string>

class NamedObject {
public:
    inline std::string Name() const {
        return m_strName;
    }

    inline void SetName(const std::string& p_Name) {
        m_strName = p_Name;
    }

protected:
    std::string m_strName;
};

#endif //AX_MEDIAPIPLE_NAMED_OBJECT_H
