#include "pwrapper.h"
#include "arrayobject.h"
#include "chimerge.h"
#include <vector>
#include <deque>
#include <ctime>



using namespace std;

PyObject *vector_to_list(vector<double>);


PyObject* fit(PyObject* self, PyObject* args)
{

	
	PyArrayObject  *x_array, *y_array;
	NpyIter_IterNextFunc *x_iternext, *y_iternext;

	if (!PyArg_ParseTuple(args, "O!O!", &PyArray_Type, 
        &x_array, &PyArray_Type, &y_array))
        return NULL;

	NpyIter *x_iter;
    x_iter = NpyIter_New(x_array, NPY_ITER_READONLY, NPY_KEEPORDER,
                             NPY_NO_CASTING, NULL);
    if (x_iter == NULL)
        return 0;
	x_iternext = NpyIter_GetIterNext(x_iter, NULL);


	NpyIter *y_iter;
    y_iter = NpyIter_New(y_array, NPY_ITER_READONLY, NPY_KEEPORDER,
                             NPY_NO_CASTING, NULL);
    if (y_iter == NULL)
        return 0;
	y_iternext = NpyIter_GetIterNext(y_iter, NULL);

	/*  iterate over the arrays */
	double ** x_dataptr = (double **) NpyIter_GetDataPtrArray(x_iter);
    int ** y_dataptr = (int **) NpyIter_GetDataPtrArray(y_iter);

	std::vector<double> *values = new std::vector<double>();
	std::vector<int> *target = new std::vector<int>();

    do {
		values->push_back(**x_dataptr);
		target->push_back(**y_dataptr);

    } while(x_iternext(x_iter) && y_iternext(y_iter));

	
	clock_t start_s=clock(); // begin
	ChiMerge<double, int> merge(values, target, values->size());
	merge.fit();
	clock_t stop_s=clock();  // end
	cout << "\nExecution time: "<< (double)(stop_s - start_s)/CLOCKS_PER_SEC << endl;

	PyObject *ret = vector_to_list(merge.ranges);

	/* Clean up. */
	Py_DECREF(x_array);
	Py_DECREF(y_array);

	
    return ret;

}


PyObject *vector_to_list(vector<double> v)
  { PyObject *pylist, *item;
    pylist = PyList_New(v.size());
    for (int i=0; i<v.size(); i++) {
      item = PyFloat_FromDouble(v.at(i));
      PyList_SetItem(pylist, i, item);
    }
    return pylist;
  }


PyObject* hello(PyObject* self, PyObject* args)
{
	return Py_BuildValue("s", "nase");
}

PyMethodDef ChiMergeMethods[] = 
{
	{"fit", (PyCFunction)fit, METH_VARARGS, 0},
	{"hello", (PyCFunction)hello, METH_VARARGS, 0},
	{NULL, NULL, 0, NULL} /* Sentinel */
};


PyMODINIT_FUNC
initchimerge(void)
{
	PyObject *m  = Py_InitModule("chimerge", ChiMergeMethods);
    if (m == NULL) return;

	/* Load `numpy` functionality. */
    import_array();

}