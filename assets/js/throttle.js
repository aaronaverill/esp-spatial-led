export default function throttle(func, delay) {
  let lastCall = 0
  let timeoutId

  return function(...args) {
    const now = Date.now()
    const remainingTime = delay - (now - lastCall)

    clearTimeout(timeoutId)

    if (remainingTime <= 0) {
      lastCall = now
      func.apply(this, args)
    } else {
      timeoutId = setTimeout(() => {
        lastCall = Date.now()
        func.apply(this, args)
      }, remainingTime)
    }
  }
}