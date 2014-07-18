# TODO

* Improve documentation.

* Only update a window's composite pixmap if there are no pending configure
requests.  This could potentially (though probably slightly, if at all) speed up
response to window resizing.

* Profile!

* Consider window shadows.  This does not fit in that well with Ortle's
"do basically nothing" theme, but it should be straightforward enough.
