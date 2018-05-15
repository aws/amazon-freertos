import abc # Abstract base class

class IotAbstract(object):
    __metaclass__ = abc.ABCMeta

    """
    A plan that can be validated and built
    """

    @abc.abstractmethod
    def __init__(self, name):
        """
        """
        pass

    @abc.abstractmethod
    def create(self):
        """
        """
        pass

    @abc.abstractmethod
    def delete(self):
        """
        """
        pass

    @abc.abstractmethod
    def exists(self):
        """
        """
        pass
