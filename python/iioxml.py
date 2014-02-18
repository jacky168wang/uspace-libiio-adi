#!/usr/bin/env python

from iio import LocalContext

def print_attr(attr, ident, value):
	print '%s<attribute name="%s" value="%s" />' % (ident * '\t', attr, value)

def print_channel(chn, ident):
	xml = '%s<channel id="%s" type="%s"' % \
			(ident * '\t', chn.id, "output" if chn.output else "input")
	if chn.name:
		xml += ' name="%s"' % chn.name
	xml += ' >'
	print xml

	for attr in chn.attrs:
		print_attr(attr, ident + 1, chn.read_attr(attr))
	print '%s</channel>' % (ident * '\t', )

def print_device(dev, ident):
	xml = '%s<device id="%s"' % (ident * '\t', dev.id)
	if dev.name:
		xml += ' name="%s"' % dev.name
	xml += ' >'
	print xml

	for chn in dev.channels:
		print_channel(chn, ident + 1)
	for attr in dev.attrs:
		print_attr(attr, ident + 1, dev.read_attr(attr))
	print '%s</device>' % (ident * '\t', )

def print_context(ctx, ident):
	print '%s<context name="%s">' % (ident * '\t', ctx.name)
	for dev in ctx.devices:
		print_device(dev, ident + 1)
	print '</context>'

def main():
	print '<?xml version="1.0" encoding="utf-8"?>'
	print '<!DOCTYPE context ['
	print '\t<!ELEMENT context (device)*>'
	print '\t<!ELEMENT device (channel | attribute)*>'
	print '\t<!ELEMENT channel (attribute)*>'
	print '\t<!ELEMENT attribute EMPTY>'
	print '\t<!ATTLIST context name CDATA #REQUIRED>'
	print '\t<!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED>'
	print '\t<!ATTLIST channel id CDATA #REQUIRED type CDATA #REQUIRED name CDATA #IMPLIED>'
	print '\t<!ATTLIST attribute name CDATA #REQUIRED value CDATA #REQUIRED>'
	print ']>'

	print_context(LocalContext(), 0)

if __name__ == '__main__':
	main()