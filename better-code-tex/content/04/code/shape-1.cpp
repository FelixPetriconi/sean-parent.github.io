class circle final : public shape {
    double _radius;
public:
    explicit circle(double radius) :
        _radius(radius)
        { }
    void draw(ostream& out, size_t position) const override {
        out << string(position, ' ');
        out << "circle: " << _radius << endl;
    }
};

class rectangle final : public shape {
    double _width;
    double _height;
public:
    explicit rectangle(double width, double height) :
        _width(width), _height(height)
        { }
    void draw(ostream& out, size_t position) const override {
        out << string(position, ' ');
        out << "rectangle: " << _width << ", " << _height << endl;
    }
};

int main() {
    document doc;
    doc.emplace_back(make_shared<circle>(42.0));
    draw(doc, cout, 0);
}