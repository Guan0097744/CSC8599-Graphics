#ifndef UNIFORMMEMORY_HPP
#define UNIFORMMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "VertexBuffer.hpp"
#include "../Shader.h"

#define N 4 // word size

namespace UBO 
{
    enum class Type : unsigned char 
    {
        SCALAR = 0,
        VEC2,
        VEC3,
        VEC4,
        ARRAY,
        STRUCT,
        INVALID
    };

    /**
     * @brief Round up val to the next multiple of 2^n
     * @param val 
     * @param n 
     * @return 
    */
    inline unsigned int RoundUpPow2(unsigned int val, unsigned char n) 
    {
        unsigned int pow2n      = 0b1 << n;     // = 1 * 2^n = 2^n
        unsigned int divisor    = pow2n - 1;    // = 0b0111...111 (n 1s)

        // last n bits = remainder of val / 2^n
        // add (2^n - rem) to get to the next multiple of 2^n
        unsigned int rem = val & divisor;
        if (rem) 
        {
            val += pow2n - rem;
        }

        return val;
    }

    typedef struct Element 
    {
        Type                    type;
        unsigned int            baseAlign;
        unsigned int            length;     // Length of the array or num elements in structure
        std::vector<Element>    list;       // For struct (list of sub-elements), or array (1st slot is the type)
    
        std::string TypeStr() 
        {
            switch (type) 
            {
            case Type::SCALAR:  return "scalar";
            case Type::VEC2:    return "vec2";
            case Type::VEC3:    return "vec3";
            case Type::VEC4:    return "vec4";
            case Type::ARRAY:   return "array<" + list[0].TypeStr() + ">";
            case Type::STRUCT:  return "struct";
            default:            return "invalid";
            };
        }

        unsigned int AlignPow2() 
        {
            switch (baseAlign) 
            {
            case 2:     return 1;
            case 4:     return 2;
            case 8:     return 3;
            case 16:    return 4;
            default:    return 0;
            };
        }

        unsigned int CalcSize() 
        {
            switch (type) 
            {
            case Type::SCALAR:  return N;
            case Type::VEC2:    return 2 * N;
            case Type::VEC3:    return 3 * N;
            case Type::VEC4:    return 4 * N;
            case Type::ARRAY:
            case Type::STRUCT:  return CalcPaddedSize();
            default:            return 0;
            };
        }

        unsigned int CalcPaddedSize() 
        {
            unsigned int offset = 0;

            switch (type) 
            {
            case Type::ARRAY:
                return length * RoundUpPow2(list[0].CalcSize(), AlignPow2());
            case Type::STRUCT:
                for (Element e : list) 
                {
                    offset = RoundUpPow2(offset, e.AlignPow2());
                    offset += e.CalcSize();
                }
                return offset;
            case Type::SCALAR:
            case Type::VEC2:
            case Type::VEC3:
            case Type::VEC4:
            default:
                return CalcSize();
            };
        }

        Element(Type type = Type::SCALAR)
            : type(type), length(0), list(0) 
        {
            switch (type) 
            {
            case Type::SCALAR:
                baseAlign = N;
                break;
            case Type::VEC2:
                baseAlign = 2 * N;
                break;
            case Type::VEC3:
            case Type::VEC4:
                baseAlign = 4 * N;
                break;
            default:
                baseAlign = 0;
                break;
            };
        }
    } Element;

    inline Element NewScalar() 
    {
        return Element();
    }

    inline Element NewVec(unsigned char dim) 
    {
        switch (dim) 
        {
        case 2: return Type::VEC2;
        case 3: return Type::VEC3;
        case 4:
        default:
            return Type::VEC4;
        };
    }

    inline Element NewArray(unsigned int length, Element arrElement) 
    {
        Element ret(Type::ARRAY);
        ret.length = length;
        ret.list = { arrElement };
        ret.list.shrink_to_fit();

        ret.baseAlign = arrElement.type == Type::STRUCT ?
            arrElement.baseAlign :
            RoundUpPow2(arrElement.baseAlign, 4);

        return ret;
    }

    inline Element NewColMat(unsigned char cols, unsigned char rows) 
    {
        return NewArray(cols, NewVec(rows));
    }

    inline Element NewColMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows) 
    {
        return NewArray(noMatrices * cols, NewVec(rows));
    }

    inline Element NewRowMat(unsigned char rows, unsigned char cols) 
    {
        return NewArray(rows, NewVec(cols));
    }

    inline Element NewRowMatArray(unsigned int noMatrices, unsigned char rows, unsigned char cols) 
    {
        return NewArray(noMatrices * rows, NewVec(cols));
    }

    inline Element NewStruct(std::vector<Element> subelements) 
    {
        Element ret(Type::STRUCT);
        ret.list.insert(ret.list.end(), subelements.begin(), subelements.end());
        ret.length = ret.list.size();

        // base alignment is largest of its subelements
        if (subelements.size()) 
        {
            for (Element e : subelements) 
            {
                if (e.baseAlign > ret.baseAlign) 
                {
                    ret.baseAlign = e.baseAlign;
                }
            }

            ret.baseAlign = RoundUpPow2(ret.baseAlign, 4);
        }

        return ret;
    }

    class UBO : public BufferObject 
    {
    public:
        Element         block;              // Root element of the UBO
        unsigned int    calculatedSize;
        GLuint          bindingPos;

        UBO(GLuint bindingPos)
            : BufferObject(GL_UNIFORM_BUFFER),
            block(NewStruct({})),
            calculatedSize(0),
            bindingPos(bindingPos) {}

        UBO(GLuint bindingPos, std::vector<Element> elements)
            : BufferObject(GL_UNIFORM_BUFFER),
            block(NewStruct(elements)),
            calculatedSize(0),
            bindingPos(bindingPos) {}

        void AttachToShader(Shader shader, std::string name)
        {
            GLuint blockIdx = glGetUniformBlockIndex(shader.id, name.c_str());
            glUniformBlockBinding(shader.id, blockIdx, bindingPos);
        }

        void InitNullData(GLenum usage)
        {
            if (!calculatedSize) 
            {
                calculatedSize = CalcSize();
            }
            glBufferData(type, calculatedSize, NULL, usage);
        }

        void BindRange(GLuint offset = 0) 
        {
            if (!calculatedSize) 
            {
                calculatedSize = CalcSize();
            }
            glBindBufferRange(type, bindingPos, val, offset, calculatedSize);
        }

        unsigned int CalcSize() 
        {
            return block.CalcPaddedSize();
        }

        void AddElement(Element element) 
        {
            block.list.push_back(element);
            if (element.baseAlign > block.baseAlign) 
            {
                block.baseAlign = element.baseAlign;
            }
            block.length++;
        }

        // iteration variables
        GLuint offset;
        GLuint poppedOffset;
        std::vector<std::pair<unsigned int, Element*>> indexStack; // stack to keep track of the nested indices
        int currentDepth; // current size of the stack - 1

        // initialize iterator
        void StartWrite() 
        {
            currentDepth    = 0;
            offset          = 0;
            poppedOffset    = 0;
            indexStack.clear();
            indexStack.push_back({ 0, &block });
        }

        // next element in iteration
        Element GetNextElement() 
        {
            // highest level struct popped, stack is empty
            if (currentDepth < 0) 
            {
                return Type::INVALID;
            }

            // get current deepest array/struct (last element in the stack)
            Element* currentElement = indexStack[currentDepth].second;

            // get the element at the specified index within that iterable
            if (currentElement->type == Type::STRUCT) 
            {
                currentElement = &currentElement->list[indexStack[currentDepth].first];
            }
            else 
            { 
                // array
                currentElement = &currentElement->list[0];
            }

            // traverse down to deepest array/struct
            while (currentElement->type == Type::STRUCT || currentElement->type == Type::ARRAY) 
            {
                currentDepth++;
                indexStack.push_back({ 0, currentElement });
                currentElement = &currentElement->list[0];
            }

            // now have current element (not an iterable)
            // pop from stack if necessary
            poppedOffset = RoundUpPow2(offset, currentElement->AlignPow2()) + currentElement->CalcSize();
            if (!pop()) 
            {
                // no items popped
                poppedOffset = 0;
            }

            return *currentElement;
        }

        bool pop() 
        {
            bool popped = false;

            for (int i = currentDepth; i >= 0; i--) 
            {
                int advancedIdx = ++indexStack[i].first; // move cursor forward in the iterable
                if (advancedIdx >= indexStack[i].second->length) 
                {
                    // iterated through entire array or struct
                    // pop iterable from the stack
                    poppedOffset = RoundUpPow2(poppedOffset, indexStack[i].second->AlignPow2());
                    indexStack.erase(indexStack.begin() + i);
                    popped = true;
                    currentDepth--;
                }
                else 
                {
                    break;
                }
            }

            return popped;
        }

        template<typename T>
        void WriteElement(T* data) 
        {
            Element element = GetNextElement();
            //std::cout << element.typeStr() << "--" << element.baseAlign << "--" << offset << "--";
            offset = RoundUpPow2(offset, element.AlignPow2());
            //std::cout << offset << std::endl;

            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);

            if (poppedOffset) 
            {
                offset = poppedOffset;
            }
            else 
            {
                offset += element.CalcSize();
            }
        }

        template<typename T>
        void WriteArray(T* first, unsigned int n) 
        {
            for (int i = 0; i < n; i++) 
            {
                WriteElement<T>(&first[i]);
            }
        }

        template<typename T, typename V>
        void WriteArrayContainer(T* container, unsigned int n) 
        {
            for (int i = 0; i < n; i++) 
            {
                WriteElement<V>(&container->operator[](i)); // container[i] translates to container + i
            }
        }

        void AdvanceCursor(unsigned int n) 
        {
            // skip number of elements
            for (int i = 0; i < n; i++) 
            {
                Element element = GetNextElement();
                offset = RoundUpPow2(offset, element.AlignPow2());
                if (poppedOffset) 
                {
                    offset = poppedOffset;
                }
                else 
                {
                    offset += element.CalcSize();
                }
            }
        }

        void AdvanceArray(unsigned int noElements) 
        {
            if (currentDepth < 0) 
            {
                return;
            }

            Element* currentElement = indexStack[currentDepth].second;

            // get the next array
            if (currentElement->type == Type::STRUCT) 
            {
                currentElement = &currentElement->list[indexStack[currentDepth].first];

                unsigned int depthAddition = 0;
                std::vector<std::pair<unsigned int, Element*>> stackAddition;

                // go to next array
                while (currentElement->type == Type::STRUCT) 
                {
                    depthAddition++;
                    stackAddition.push_back({ 0, currentElement });
                    currentElement = &currentElement->list[0];
                }

                if (currentElement->type != Type::ARRAY) 
                {
                    // did not find an array (reached primitive)
                    return;
                }

                // found array, apply changes
                currentDepth += depthAddition + 1; // + 1 for the array
                indexStack.insert(indexStack.end(), stackAddition.begin(), stackAddition.end());
                indexStack.push_back({ 0, currentElement }); // push array to stack
            }

            // at an array, advance number of elements
            unsigned int finalIdx = indexStack[currentDepth].first + noElements;
            unsigned int advanceCount = noElements;
            if (finalIdx >= indexStack[currentDepth].second->length) 
            {
                // advance to the end of array
                advanceCount = indexStack[currentDepth].second->length - indexStack[currentDepth].first;
            }

            // advance offset
            offset += advanceCount * RoundUpPow2(currentElement->list[0].CalcSize(), currentElement->AlignPow2());
            // advance cursor in stack
            indexStack[currentDepth].first += advanceCount;

            // pop from stack
            poppedOffset = offset;
            if (pop()) 
            {
                // item(s) popped
                offset = poppedOffset;
            }
        }
    };
};

#endif