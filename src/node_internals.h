// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_NODE_INTERNALS_H_
#define SRC_NODE_INTERNALS_H_

namespace node {

// This function starts an Isolate. This function is defined in node.cc
// currently so that we minimize the diff between master and v0.6 for easy
// merging. In the future, when v0.6 is extinct, StartThread should be moved
// to node_isolate.cc.
class Isolate;
void StartThread(Isolate* isolate, int argc, char** argv);

#ifndef offset_of
// g++ in strict mode complains loudly about the system offsetof() macro
// because it uses NULL as the base address.
#define offset_of(type, member) \
  ((intptr_t) ((char *) &(((type *) 8)->member) - 8))
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offset_of(type, member)))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

//
// isolates support
//
#if HAVE_ISOLATES

# if _WIN32
#  define THREAD __declspec(thread)
# else
#  define THREAD __thread
# endif

# define TLS(type, name)      THREAD type* __tls_##name
# define VAR(name)            (*__tls_##name)
# define EMPTY(name)          (__tls_##name == NULL)
# define ASSIGN(name, val)    ((__tls_##name) = P(val))

# define LAZY_ASSIGN(name, val) \
  do if (!__tls_##name) ((__tls_##name) = P(val)); while (0)

template <class T> inline v8::Persistent<T>* P(v8::Handle<T> v)
{
  return new v8::Persistent<T>(v8::Persistent<T>::New(v));
}

inline v8::Persistent<v8::String>* P(const char* symbol)
{
  return new v8::Persistent<v8::String>(
    v8::Persistent<v8::String>::New(
      v8::String::NewSymbol(symbol)));
}

#else // !HAVE_ISOLATES

# define THREAD             /* nothing */
# define TLS(type, name)    type name
# define VAR(name)          (name)
# define EMPTY(name)        ((name).IsEmpty())
# define ASSIGN(name, val)  ((name) = P(val))

# define LAZY_ASSIGN(name, val) \
  do if ((name).IsEmpty()) (name) = P(val); while (0)

template <class T> inline v8::Persistent<T> P(v8::Handle<T> v)
{
  return v8::Persistent<T>(v);
}

inline v8::Persistent<v8::String> P(const char* symbol)
{
  return v8::Persistent<v8::String>::New(
    v8::String::NewSymbol(symbol));
}
#endif // HAVE_ISOLATES

} // namespace node

#endif // SRC_NODE_INTERNALS_H_
