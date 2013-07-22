#ifndef SERIAL_INTERFACE_HH
#define SERIAL_INTERFACE_HH

/* This library is used to communicate with the computer over a standard
   interface used for doing scientific measurements.  The following message
   types are available:
   Host to device:
   1. Change value of variable (int, float, string).
   2. Read value of sensor.
   3. Request to be notified on event.
   4. Event. (Special case: reset event.)
   5. Get interface definition.
   Device to host:
   1. Interface definition.
   2. Sensor value reply.
   3. Event.

   This means that the device must contain an interface definition.  This
   header file defines methods for defining it, and for using the system.

   An interface consists of any number of variables, sensors, events and
   objects.  Objects are themselves interfaces which consist of any of those
   elements.  In addition, an object can define to be of a class, which means
   it will follow a standard protocol.

   All interface variables must be defined as globals.
 */

extern List <String> _strings;
extern List <_Input> _inputs;
extern List <_Output> _outputs;
extern int _next_id;

class String {
	unsigned l_length;
	char const *data;
	public:
	String (int x) : l_length (0), data (NULL) {}
	String (char const *message) : l_length (strlen (message)) {
		data = malloc (l_length);
		memcpy (data, message);
	}
	String (char const *message, unsigned length) : l_length (length) {
		data = malloc (l_length);
		memcpy (data, message);
	}
	operator char const *() { return data; }
};

template <typename T> class List;

template <typename T> class ListItem {
	friend class List <T>;
	ListItem *prev, *next;
	T data;
	public:
	ListItem (T &source, List <T> *parent) : prev (parent->last), next (NULL), data (source) {
		if (prev)
			prev->next = this;
		else
			parent->first = this;
		parent->last = this;
	}
	T &operator* () { return data; }
};

template <typename T> class List {
	int l_length;
	ListItem <T> *first, *last;
	T **array;
	public:
	List () : l_length (0), first (NULL), last (NULL), array (NULL);
	void push_back (T &item) { ListItem (item, this); }
	T &back () { return **last; }
	int length () const { return l_length; }
	void freeze () {
		array = malloc (sizeof (T) * l_length);
		int t = 0;
		for (ListItem <T> *p = first; p; p = p->next, ++t)
			array[t] = &p->data;
	}
	T &operator[] (int index) { return *array[index]; }
};

enum _type { _INT, _FLOAT, _STRING };

#define InInt(name) void _change_ ## name (); int name = _register_in (#name, &name, _change_ ## name, _parse_int, _INT); void _change_ ## name ()
#define InFloat(name) void _change_ ## name (); float name = _register_in (#name, &name, _change_ ## name, _parse_float, _FLOAT); void _change_ ## name ()
#define InString(name) void _change_ ## name (); String name = _register_in (#name, &name, _change_ ## name, _parse_string, _STRING); void _change_ ## name ()
#define OutInt(name) void name (int arg) { _write_int (_id_ ## name, arg); } int _id_ ## name = _register_out (#name, _INT);
#define OutFloat(name) void name (float arg) { _write_float (_id_ ## name, arg); } int _id_ ## name = _register_out (#name, _FLOAT);
#define OutString(name) void name (String const &arg) { _write_string (_id_ ## name, arg); } int _id_ ## name = _register_out (#name, _STRING);

struct _Input {
	int id;
	String *name;
	void *data;
	void (*cb) ();
	void *(*parser) (int len, char const *message, void *data);
	_type type;
	_Input (int id, void *data, String *name, void (*cb)(), void *(*parser)(char const *message), _type type) : id (id), data (data), name (name), cb (cb), arg (arg), parser (parser), type (type) {}
};

struct _Output {
	int id;
	String *name;
	_type type;
	_Output (int id, String *name, _type type) : id (id), name (name), type (type) {}
};

static int _register_in (name, cb, parser, type) {
	_strings.push_back (name);
	_inputs.push_back (_Input (_next_id++, &_strings.back (), cb, parser, type));
	return 0;
}

static int _register_out (name, type) {
	_strings.push_back (name);
	_outputs.push_back (_Output (_next_id++, &_strings.back (), type));
	return 0;
}

static void _parse_int (int len, char const *message, void *data) {
	*reinterpret_cast <int *> (data) = *reinterpret_cast <int *> (message);
}

static void _parse_float (int len, char const *message, void *data) {
	*reinterpret_cast <float *> (data) = *reinterpret_cast <float *> (message);
}

static void _parse_string (int len, char const *message) {
	_strings.push_back (String (message, len));
	*reinterpret_cast <String *> (data) = &_strings.back ();
}

static void _write_int (int id, int arg)
{
}

static void _write_float (int id, float arg)
{
}

static void _write_string (int id, String const &arg)
{
}

char *_serialbuffer[0x100];
int _buffersize = 0;

void serialEvent ()
{
	_serialbuffer[_buffersize++] = Serial.read ();
	while (_serialbuffer[0] > _buffersize - 1 && Serial.available ())
	{
		_serialbuffer[_buffersize++] = Serial.read ();
		if (_serialbuffer[0] == _buffersize - 1)
		{
			// Handle buffer. TODO
		}
	}
}

#endif
