from . import TestCase, main


class TestBasics(TestCase):

    def test(self):
        assert (1 < 2)


if __name__ == '__main__':
    main()
