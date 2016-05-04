root = exports ? this

## functional programming?
id = (x) -> x
nop = ->
flatten = (xs) ->
  flat = [].concat.apply [], xs
  if xs.some Array.isArray then flatten flat else flat

Array.prototype.some ?= (f) ->
  (return true if f x) for x in @
  return false

Array.prototype.every ?= (f) ->
  (return false if not f x) for x in @
  return true

max = (ar, fn = id) ->
  if ar
    bst = ar[0]
    ar.forEach (x) ->
      if fn(x) > fn(bst)
        bst = x
    bst

min = (ar, fn = id) ->
  if ar
    bst = ar[0]
    ar.forEach (x) ->
      if fn(x) < fn(bst)
        bst = x
    bst

## convenience functions
rand = (n) ->
  Math.floor(Math.random() * n)

add = (x, y) -> x + y

sum = (lst) -> lst.reduce(add)

clone = (obj) ->
  if not obj? or typeof obj isnt 'object'
    return obj

  if obj instanceof Date
    return new Date(obj.getTime()) 

  if obj instanceof RegExp
    flags = ''
    flags += 'g' if obj.global?
    flags += 'i' if obj.ignoreCase?
    flags += 'm' if obj.multiline?
    flags += 'y' if obj.sticky?
    return new RegExp(obj.source, flags) 

  newInstance = new obj.constructor()

  for key of obj
    newInstance[key] = clone obj[key]

  return newInstance

## debugging
print = ->
  console.log.apply(console, arguments)

error = ->
  console.error.apply(console, arguments)

root.print = print
root.error = error
root.clone = clone
root.max = max
root.min = min
root.rand = rand
root.sum = sum
root.flatten = flatten