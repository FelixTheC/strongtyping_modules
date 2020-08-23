# Strongtyping Module
An optional extension for a performance boost for the StrongTyping package https://github.com/FelixTheC/strongtyping<br>
This optional module can boost the performance  by __factor 3__ for simple/single function parameters<br>
and by __factor 13__ for more complex/multiple function parameters.


## Installation
- This package is build on Cython
- therefore a C-Compiler is required https://cython.readthedocs.io/en/latest/src/quickstart/install.html
- when both requirements are available run somewhere in your project (once)
```python
from strongtyping_modules import install

install()
```
- after this the module is available and will be used automatically from strongtyping

#### For displaying the performance impact of this optional module I created the following test functions. 

```python
@match_typing
def func_with_list(var: List[int]):
    return len(var)


@match_typing
def func_with_set(var: Set[int]):
    return len(var)


@match_typing
def func_with_dict(var: Dict[str, int]):
    return len(var)


@match_typing
def func_with_dict_list(var_1: Dict[str, int], var_2: List[str]):
    return len(var_1) + len(var_2)


@match_typing
def func_list_with_union_1_level(var: List[Union[str, int]]):
    return len(var)
```

I called each function inside of a loop and with each iteration I used 'i' elements
```python
for i in range(10, 10010, 10):
    func_with_list(list(range(i)))
```

Here are the results without these module
```python
{'func_list_with_union_1_level': {'max': 0.0824721889985085,
                                  'mean': 0.040940183910031916,
                                  'median': 0.04097814700071467,
                                  'min': 0.00011775800157920457},
 'func_with_dict': {'max': 0.033724983000865905,
                    'mean': 0.01682099703201311,
                    'median': 0.01687698050045583,
                    'min': 5.355300163500942e-05},
 'func_with_dict_list': {'max': 0.3036283949986682,
                         'mean': 0.025529991152990986,
                         'median': 0.02529048100041109,
                         'min': 0.0001045319986587856},
 'func_with_list': {'max': 0.01744058300027973,
                    'mean': 0.008482154103028734,
                    'median': 0.008478498000840773,
                    'min': 3.4947999665746465e-05},
 'func_with_set': {'max': 0.01736680899921339,
                   'mean': 0.008591252664042258,
                   'median': 0.008604455000750022,
                   'min': 3.981400004704483e-05}}

```

Ande here with these module
```python
{'func_list_with_union_1_level': {'max': 0.017030322000209708,
                                  'mean': 0.008434945453987893,
                                  'median': 0.008279206000224804,
                                  'min': 4.34969988418743e-05},
 'func_with_dict': {'max': 0.004060829000081867,
                    'mean': 0.0020102006380620877,
                    'median': 0.0020167855018371483,
                    'min': 1.697399784461595e-05},
 'func_with_dict_list': {'max': 0.007045086000289302,
                         'mean': 0.003219366065990471,
                         'median': 0.003227623998100171,
                         'min': 2.7607999072642997e-05},
 'func_with_list': {'max': 0.0025434739982301835,
                    'mean': 0.0012344250298920087,
                    'median': 0.001237124501130893,
                    'min': 1.1835003533633426e-05},
 'func_with_set': {'max': 0.002797465000185184,
                   'mean': 0.0012589457310496074,
                   'median': 0.0012730154994642362,
                   'min': 1.298200004384853e-05}}

```

x Faktor faster for different functions
```python
min       4.178648
mean      4.847373
median    4.796414
max       4.760371
Name: func_list_with_union_1_level, dtype: float64

min       3.177067
mean      7.067269
median    7.284502
max       3.898747
Name: func_with_list, dtype: float64

min       3.103697
mean      6.708103
median    6.790693
max       4.583332
Name: func_with_set, dtype: float64

min       3.303017
mean      8.656822
median    8.794840
max       3.579946
Name: func_with_dict, dtype: float64

min        3.368734
mean       8.407030
median     8.697711
max       13.022075
Name: func_with_dict_list, dtype: float64
```

### Dependencies
- Cython

### Tested for OS
- Linux, should theoretical work on Windows and MacOs too

### Installing
- pip install strongtyping-modules

#### Versioning
- For the versions available, see the tags on this repository.

### Authors
- Felix Eisenmenger

### License
- This project is licensed under the MIT License - see the LICENSE.md file for details
